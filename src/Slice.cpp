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
    } else if (memb == &Forward::bid){
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
    if( fwd_price && value > 0 ){
        Option* opt = dynamic_cast<Option*>(instrument);
        imply_vol_queue->addTask([&opt,&fwd_price,&memb]{opt->work_after_update( fwd_price, memb );});
    }
    //work_after_update(memb);
}