#include "Slice.h"
#include "Contract.h"
#include "Pricing.h"
#include <functional>
#include "Date.h"
#include <thread>
#include "TaskQueue.h"
#include <fstream>

using  namespace std::placeholders;
using std::bind;
using std::function;
using std::endl;
using std::next;
using std::prev;

Slice::Slice():imply_vol_queue(new TaskQueue),imply_vol_t(workerThread, imply_vol_queue),best_synth_prices{-1,-1},log("log/slice.txt"){
}

Slice::~Slice(){
    imply_vol_queue->stop();
    if(imply_vol_t.joinable())
        imply_vol_t.join();
    delete imply_vol_queue; 
}

void Slice::assign_forward( const ContractDetails& c){
    if( c.contractMonth == contractMonth || forward.expiry_str == "" || forward.expiry_str>c.realExpirationDate){
        forward.contract = c.contract;
        forward.expiry_str = c.realExpirationDate;
        forward.expiry = Date::create_from_string( c.lastTradeTime, c.realExpirationDate, c.contract.exchange );
        forward.right = Forward::FORWARD;
    }
}

void Slice::assign_option(const ContractDetails& c){
    const float s = c.contract.strike;
    const Option::Right r = c.contract.right == "C" ? Option::CALL : Option::PUT;
    Option& o = options[s][r];
    o.contract = c.contract;
    o.expiry_str = c.realExpirationDate;
    o.expiry = Date::create_from_string( c.lastTradeTime, c.realExpirationDate, c.contract.exchange );
    o.strike = s;
    o.right = r;
}

void Option::work_after_update( const double& fwd_price, double Forward::* memb ){
    double Option::* memb_vol;
    time_to_maturity = (expiry - last_update) / (364.2425 * 24 * 60 * 60); 
    if( memb == &Forward::ask){
        memb_vol = &Option::vol_ask;
    } else {
        memb_vol = &Option::vol_bid;
    }
    function<double(double)> f = bind( black_formula, right, fwd_price, strike, _1, time_to_maturity, 1 );
    function<double(double)> d = bind( vega, fwd_price, strike, _1, time_to_maturity, 1 );
    this->*memb_vol = newton_method( f, d,this->*memb, 0.2, 0.0001, 10 );
}

void Slice::update_float_memb( Forward * instrument,const int field,const double value){
    double fwd_price = 0;
    double Forward::* memb;
    instrument->last_update = time(nullptr);
    switch (field)
    {
    case 66: //delayed bid
        memb = &Forward::bid;
        break;
    case 67: //delayed ask
        memb = &Forward::ask;
        break;
    case 68: //delayed last
        memb = &Forward::last;
        break;
    case 1: //bid
        memb = &Forward::bid;
        break;
    case 2: //ask
        memb = &Forward::ask;
        break;
    case 4: //last
        memb = &Forward::last;
        break;
    default:
        return;
    } 
    if( (instrument->right == Forward::CALL && memb == &Forward::ask ) || (instrument->right == Forward::PUT && memb == &Forward::bid ) )
        fwd_price = forward.ask;
    else if( (instrument->right == Forward::CALL && memb == &Forward::bid ) || (instrument->right == Forward::PUT && memb == &Forward::ask ) )
        fwd_price = forward.bid;
    instrument->*memb = value;

    if( (memb == &Forward::ask || memb == &Forward::bid) && ( instrument->right == Forward::CALL ||  instrument->right == Forward::PUT )){
        Option* opt = dynamic_cast<Option*>(instrument);
        Option* put,* call;
        float strike = opt->strike;
        updated_strikes.insert(strike);
        if( fwd_price && value > 0  ){ // Imply new vol on the imply_vol_thread
            imply_vol_queue->addTask([opt,fwd_price,memb]{opt->work_after_update( fwd_price, memb );});  
            log << "Queue imply vol: " << imply_vol_queue->size() << endl;
        }
        if( opt->right == Option::CALL){
            call = opt;
            put = &(this->options[opt->strike][Option::PUT]);
        } else{
            call = &(this->options[opt->strike][Option::CALL]);
            put = opt;
        }
        synth_prices[strike] = { -1,-1};
        if(call->bid > -1 && put->ask > -1 )
            synth_prices[strike].first = call->bid-put->ask + strike;
        if(call->ask > -1 && put->bid> -1 )
            synth_prices[strike].second = call->ask-put->bid + strike;
    }
}

void Slice::check_fly_aux(Option* l_opt,Option* m_opt,Option* h_opt){
    float l_ask = l_opt->ask;
    float m_bid = m_opt->bid; 
    float h_ask = h_opt->ask;
    float fly = l_ask - 2 * m_bid + h_ask;
    if(l_ask > -1 && m_bid > -1 && h_ask > -1 && fly <= 0){
        //send orders for fly  
        m_pClient->placeOrderFly(l_opt,m_opt,h_opt);
    }
}

void Slice::check_fly(){
    // assuming we have more than 4 strikes so second != second_last always
    float fwd_mid = (forward.bid + forward.ask ) / 2;
    for(const float& strike : updated_strikes){
        const Option::Right right = strike>fwd_mid?Option::CALL:Option::PUT;
        const auto it = options.find(strike), beg = options.begin(), end = options.end();
        const auto second = next(beg);
        const auto last = prev(end);
        const auto second_last = prev(last);
        if(it != options.end()){
            if(it != beg && it != second){
                float p_strike = prev(it)->first; 
                float pp_strike = prev(prev(it))->first;
                check_fly_aux(&options[pp_strike][right],&options[p_strike][right],&options[strike][right]);
            }
            if(it != last && it != second_last){
                float n_strike = next(it)->first;
                float nn_strike = next(next(it))->first;
                check_fly_aux(&options[strike][right],&options[n_strike][right],&options[nn_strike][right]);
            }
            if(it != beg && it != last ){
                float n_strike = next(it)->first; 
                float p_strike = prev(it)->first; 
                check_fly_aux(&options[p_strike][right],&options[strike][right],&options[n_strike][right]);
            }
        }
    }
}

void Slice::update_synthetic(){
    for(const float& strike : updated_strikes){
        
        best_synth.second.erase( strike);
        if(synth_prices[strike].first > -1){
            if(synth_prices[strike].first > best_synth_prices.first ){
                best_synth.first.clear();
                best_synth.first.insert(strike);
                best_synth_prices.first = synth_prices[strike].first; 
            } else if(synth_prices[strike].first  == best_synth_prices.first){
                best_synth.first.insert(strike);
            } else if(best_synth.first.find( strike) != best_synth.first.end()){
                best_synth.first.erase( strike);
            }
            if( !best_synth.first.size()){
                best_synth_prices.first  = -1;
            }
        }
        if(synth_prices[strike].second > -1 ){
            if( best_synth_prices.second == -1 || synth_prices[strike].second < best_synth_prices.second ){
                best_synth.second.clear();
                best_synth.second.insert(strike);
                best_synth_prices.second = synth_prices[strike].second;
            } else if(synth_prices[strike].second  == best_synth_prices.second){
                best_synth.second.insert(strike);
            } else if(best_synth.second.find( strike) != best_synth.second.end()){
                best_synth.second.erase( strike);
            }
            if( !best_synth.second.size()){
                best_synth_prices.second  = -1;
            }
        }
    }
    if( best_synth_prices.second > -1 && forward.bid > best_synth_prices.second ){
        // buy the synth from best_synth.first and sell the futiure
        m_pClient->sendSynth( true);
    }
    if( forward.ask < best_synth_prices.first ){
        // sell the synth from best_synth.second and buy the futiure
        m_pClient->sendSynth( false);
    }

}