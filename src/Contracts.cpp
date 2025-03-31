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

Contract MyContract::FUTURE(string symbol, string exchange, string tradingClass, string currency){
	Contract contract;
	contract.symbol = symbol;
	contract.secType = "FUT";
	contract.exchange = exchange;
	contract.currency = currency;
	contract.tradingClass = tradingClass;
	return contract;
}

Contract MyContract::FLY(Contract& l_c,Contract& m_c, Contract& h_c){
	//! [bagoptcontract]
	Contract contract;
	contract.symbol = l_c.symbol;
	contract.secType = l_c.secType;
	contract.currency = l_c.currency;
	contract.exchange = l_c.exchange;

	ComboLegSPtr leg1(new ComboLeg);
	leg1->conId = l_c.conId;
	leg1->action = "BUY";
	leg1->ratio = 1;
	leg1->exchange = contract.exchange;

	ComboLegSPtr leg2(new ComboLeg);
	leg2->conId = m_c.conId;
	leg2->action = "SELL";
	leg2->ratio = -2;
	leg2->exchange = contract.exchange;

	ComboLegSPtr leg3(new ComboLeg);
	leg3->conId = h_c.conId;
	leg3->action = "BUY";
	leg3->ratio = 1;
	leg3->exchange = contract.exchange;

	contract.comboLegs.reset(new Contract::ComboLegList());
	contract.comboLegs->push_back(leg1);
	contract.comboLegs->push_back(leg2);
	contract.comboLegs->push_back(leg3);
	return contract;
}