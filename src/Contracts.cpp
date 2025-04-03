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
	contract.tradingClass = l_c.tradingClass;

	ComboLegSPtr leg1(new ComboLeg);
	leg1->conId = l_c.conId;
	leg1->action = "BUY";
	leg1->ratio = 1;
	leg1->exchange = contract.exchange;

	ComboLegSPtr leg2(new ComboLeg);
	leg2->conId = m_c.conId;
	leg2->action = "SELL";
	leg2->ratio = 2;
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

Contract MyContract::SYNTH( const Contract& fwd, const Contract& call, const Contract& put ){
	//! [bagoptcontract]
	Contract contract;
	contract.symbol = fwd.symbol;
	contract.secType = fwd.secType;
	contract.currency = fwd.currency;
	contract.exchange = fwd.exchange;
	contract.tradingClass = fwd.tradingClass;
	contract.localSymbol = fwd.localSymbol;
	//contract.right = call.right;
	//contract.strike = call.strike;
	//contract.lastTradeDateOrContractMonth = call.lastTradeDateOrContractMonth;

	ComboLegSPtr leg1(new ComboLeg);
	leg1->conId = fwd.conId;
	leg1->action = "SELL";
	leg1->ratio = 1;
	leg1->exchange = fwd.exchange;
	
	ComboLegSPtr leg2(new ComboLeg);
	leg2->conId = call.conId;
	leg2->action = "BUY";
	leg2->ratio = 1;
	leg2->exchange = call.exchange;

	ComboLegSPtr leg3(new ComboLeg);
	leg3->conId = put.conId;
	leg3->action = "SELL";
	leg3->ratio = 1;
	leg3->exchange = put.exchange;
    
	
	contract.comboLegs.reset(new Contract::ComboLegList());
	
	//contract.comboLegs->push_back(leg1);
	contract.comboLegs->push_back(leg2);
	contract.comboLegs->push_back(leg3);
	return contract;
}