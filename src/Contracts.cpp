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
Contract MyContract::OptionWithTradingClass(){
	//! [optcontract_tradingclass]
	Contract contract;
	contract.symbol = "SANT";
	contract.secType = "OPT";
	contract.exchange = "MEFFRV";
	contract.currency = "EUR";
	contract.lastTradeDateOrContractMonth = "20190621";
	contract.strike = 7.5;
	contract.right = "C";
	contract.multiplier = "100";
	contract.tradingClass = "SANEU";
	//! [optcontract_tradingclass]
	return contract;
}