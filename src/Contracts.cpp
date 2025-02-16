#include "StdAfx.h"
#include "Contracts.h"
#include "Contract.h"
#include <string>

using std::string;

Contract MyContract::OPTION_SLICE(string symbol, string expiration, string exchange, string currency){
	Contract contract;
	contract.symbol = symbol;
	contract.secType = "OPT";
	contract.exchange = exchange;
	contract.currency = currency;
	contract.lastTradeDateOrContractMonth = expiration;
	//contract.tradingClass = "OESX";
	return contract;
}

Contract MyContract::FUTURE(string symbol, string exchange, string currency){
	Contract contract;
	contract.symbol = symbol;
	contract.secType = "FUT";
	contract.exchange = exchange;
	contract.currency = currency;
	return contract;
}