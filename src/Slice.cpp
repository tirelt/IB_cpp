#include "Slice.h"
#include "Contract.h"
#include "Pricing.h"
#include <functional>
#include "Date.h"
#include <thread>
#include "TaskQueue.h"

using  namespace std::placeholders;
using std::bind;
using std::function;

Slice::Slice():imply_vol_queue(new TaskQueue),imply_vol_t(workerThread, imply_vol_queue){
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
void Slice::update_synthetic(bool long_side, Option* call, Option* put){
    double call_price = call->ask;
    double put_price = put->bid;
    Synthetic* synth = &long_synth;
    auto comp = [&long_side](double a, double b){return (long_side?a<b:a>b);};
    double ref_price = forward.bid;
    if(!long_side){
        call_price = call->bid;
        put_price = put->ask;
        synth = &short_synth;
        ref_price = forward.ask;
    }
    double price = call_price-put_price+call->strike;
    if( synth->price == -1 || comp(price,synth->price)){
        synth->price = price;
        synth->call = call;
        synth->put = put; 
        if(comp(price,ref_price)){
            synthetic_arb = synth;
        }
    } 
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

    if( instrument->right == Forward::FORWARD){
        if(memb == &Forward::ask)
            short_synth.forward_price = value;        
        else if(memb == &Forward::bid)
            long_synth.forward_price = value;        
    } else if( (memb == &Forward::ask || memb == &Forward::bid) && ( instrument->right == Forward::CALL ||  instrument->right == Forward::PUT )){
        Option* opt = dynamic_cast<Option*>(instrument);
        Option* put,* call;
        bool long_side = true;
        if( fwd_price && value > 0  ){
            imply_vol_queue->addTask([opt,fwd_price,memb]{opt->work_after_update( fwd_price, memb );});  
        }
        if( opt->right == Option::CALL){
            call = opt;
            put = &(this->options[opt->strike][Option::PUT]);
            if(memb == &Forward::bid)
                long_side = false;
        } else{
            call = &(this->options[opt->strike][Option::CALL]);
            put = opt;
            if(memb == &Forward::ask)
                long_side = false;
        }
        update_synthetic(long_side, call, put);
    //work_after_update(memb);
    }
}