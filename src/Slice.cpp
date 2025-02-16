#include "Slice.h"
#include "Contract.h"

void Forward::assign_contract(const Contract& c){
    if( expiry=="" || expiry>c.lastTradeDate){
        contract = c;
        expiry = contract.lastTradeDate;
    }
}