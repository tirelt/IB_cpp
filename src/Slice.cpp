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