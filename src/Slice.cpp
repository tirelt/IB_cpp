#include "Slice.h"
#include "Contract.h"

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

void Forward::update_float_memb(const int field,const double value){
    switch (field)
    {
    case 66: //delayed bid
        bid = value;
        break;
    case 67: //delayed ask
        ask = value;
        break;
    case 68: //delayed last
        last = value;
        break;
    case 1: //bid
        bid = value;
        break;
    case 2: //ask
        ask = value;
        break;
    case 4: //last
        last = value;
        break;
    }
}