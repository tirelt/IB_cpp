#include "Slice.h"
#include "Contract.h"
#include "Pricing.h"
#include <functional>
#include "Date.h"

using  namespace std::placeholders;
using std::bind;

void Slice::assign_forward( const ContractDetails& c){
    if( forward.expiry_str=="" || forward.expiry_str>c.realExpirationDate){
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
    //std::function<double(double)> f,d;
    //f = bind(black_formula,)    double ( Option::Right right, double forward, double strike, double vol, double time_to_maturity, double discount ){
    if( memb == &Forward::ask){
        if(right == Option::CALL){

        } 
    } else if (memb == &Forward::bid){

    }
}
void Slice::update_float_memb( Forward * instrument,const int field,const double value){
    double fwd_price = 0;
    if(instrument->right == Forward::PUT)
        fwd_price = forward.ask;
    else
        fwd_price = forward.bid;
    switch (field)
    {
    case 66: //delayed bid
        instrument->bid = value;
        break;
    case 67: //delayed ask
        instrument->ask = value;
        break;
    case 68: //delayed last
        instrument->last = value;
        break;
    case 1: //bid
        instrument->bid = value;
        break;
    case 2: //ask
        instrument->ask = value;
        break;
    case 4: //last
        instrument->last = value;
        break;
    default:
        return;
    } 

    //work_after_update(memb);
}