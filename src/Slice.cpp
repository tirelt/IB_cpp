#include "Slice.h"
#include "Contract.h"
#include "Pricing.h"
#include <functional>

using  namespace std::placeholders;
using std::bind;

void Slice::assign_forward(const Contract& c){
    if( forward.expiry=="" || forward.expiry>c.lastTradeDate){
        forward.contract = c;
        forward.expiry = c.lastTradeDate;
    }
}

void Slice::assign_option(const Contract& c){
    const float s = c.strike;
    const Option::Right r = c.right == "C" ? Option::CALL : Option::PUT;
    Option& o = options[s][r];
    o.contract = c;
    o.expiry = c.lastTradeDate;
    o.strike = s;
    o.right = r;
}

void Option::work_after_update( double Forward::* memb ){
    //std::function<double(double)> f,d;
    //f = bind(black_formula,)    double ( Option::Right right, double forward, double strike, double vol, double time_to_maturity, double discount ){
    double Forward::* fwd_price;
    if( memb == &Forward::ask){
        if(right == Option::CALL){

        } 
    } else if (memb == &Forward::bid){

    }
}
void Slice::update_float_memb( Forward * fwd,const int field,const double value){
    double Forward::* memb;
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
    }
    fwd->*memb = value;
    //work_after_update(memb);
}