#include "StdAfx.h"
#include "Contracts.h"
#include "Contract.h"
#include <string>

using std::string;

Contract MyContract::OPTION_SLICE(string trading_class, string expiration){
	Contract contract;
	contract.secType = "OPT";
	contract.exchange = "EUREX";
	contract.currency = "EUR";
	contract.lastTradeDateOrContractMonth = expiration;
	contract.tradingClass = "OESX";
	return contract;
}
