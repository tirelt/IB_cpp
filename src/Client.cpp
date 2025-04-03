#include "StdAfx.h"
#include "Client.h"
#include "EClientSocket.h"
#include "EPosixClientSocketPlatform.h"

#include <stdio.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <ctime>
#include <fstream>
#include <cstdint>
#include "Utils.h"
#include "Contracts.h"
#include "stdexcept"
#include "memory"
#include "TaskQueue.h"
#include "Position.h"
#include "Orders.h"
#include "Order.h"
#include "OrderState.h"

using std::shared_ptr;
using std::map;
using std::pair;
using std::string;

Client::Client( shared_ptr<Slice> pSlice, shared_ptr<map<long,Position>> positions, shared_ptr<map<long,LiveOrder>> orders):
		m_osSignal(2000)//2-seconds timeout
		, m_pClient(new EClientSocket(this, &m_osSignal))
		, m_state(ST_CONNECT)	
		, m_sleepDeadline(0)
		, m_orderId(0)
		, m_extraAuth(false)
		, m_pSlice(pSlice)
		, m_pPositions(positions)
		, m_pOrders(orders)
		, log("log/client.txt"){
	m_pSlice->m_pClient = this;
	//m_pClient->reqGlobalCancel();
}
//! [socket_init]
Client::~Client(){
	// destroy the reader before the client
	if( m_pReader )
		m_pReader.reset();
	// cancel subscriptions to positions and mktdata and orders
	m_pClient->reqGlobalCancel();
	m_pClient->cancelPositions();
	for(const auto& [id,fwd]:m_pSlice->reqid_to_instrument)
		m_pClient->cancelMktData(id);
	delete m_pClient;
}

bool Client::connect(const char *host, int port, int clientId){
	// trying to connect
	printf( "Connecting to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);
	
	//! [connect]
	bool bRes = m_pClient->eConnect( host, port, clientId, m_extraAuth);
	//! [connect]
	
	if (bRes) {
		printf( "Connected to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), clientId);
		//! [ereader]
		m_pReader = std::unique_ptr<EReader>( new EReader(m_pClient, &m_osSignal) );
		m_pReader->start();
		//! [ereader]
	}
	else
		printf( "Cannot connect to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), clientId);

	return bRes;
}

void Client::disconnect() const{
	m_pClient->eDisconnect();

	printf ( "Disconnected\n");
}

bool Client::isConnected() const{
	return m_pClient->isConnected();
}

void Client::processMessages(){
	switch (m_state) {
		case ST_REQPOSITIONS:
			reqPositions();
			break;
		case ST_REQFIRSTFUT:
			reqFirstFut();
			break;
		case ST_REQSLICE:
			reqSlice();
			break;
		case ST_REQMKTDATA:
			reqMktData();
			break;
		case ST_DISCONNECT:
			disconnect();
			return;
		case ST_DUMMY_ORDER:
			dummy_order();
		case ST_ACK:
			//const unsigned sleep(1);
			//std::this_thread::sleep_for(std::chrono::microseconds(10));
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			printf("\n\ncycle ACK\n\n");
			break;
	}
	log << "Number of mesages: " << m_pReader->getQueueSize() << std::endl;
	m_osSignal.waitForSignal();
	errno = 0;
	m_pReader->processMsgs();
	log << "Number of mesages after process: " << m_pReader->getQueueSize() << std::endl;
	m_pSlice->check_fly();
	m_pSlice->update_synthetic();
}

void Client::dummy_order(){
	std::this_thread::sleep_for(std::chrono::seconds(3));
	Contract dummy_contract = MyContract::FUTURE("ESTX50","EUREX","FSXE","EUR");
	dummy_contract.lastTradeDateOrContractMonth = "202506";
	long long qty = DecimalFunctions::doubleToDecimal(1); 
	Order order = MyOrder::MARKET_ORDER("BUY", qty);
	m_pClient->placeOrder(m_orderId++,dummy_contract,order);	
	order = MyOrder::LIMIT_ORDER("BUY", qty,5150);
	//m_pClient->placeOrder(m_orderId++,dummy_contract,order);	
	m_state = ST_ACK; 	
}

void Client::nextValidId( OrderId orderId){
	printf("Next Valid Id: %ld\n", orderId);
	m_orderId = orderId;
	m_state = ST_REQPOSITIONS;
	std::this_thread::sleep_for(std::chrono::seconds(1)); //to let the first cycle to capture all the starting messages
}

void Client::connectAck(){
	if (!m_extraAuth && m_pClient->asyncEConnect())
        m_pClient->startApi();
}

void Client::reqPositions(){
	printf("Requesting Positions");
	m_pClient->reqPositions();
	m_state = ST_REQFIRSTFUT; 
	//m_state = ST_DUMMY_ORDER; 
}

void Client::position( const std::string& account, const Contract& contract, Decimal position, double avgCost) {
    printf( "Position. %s - Symbol: %s, SecType: %s, Currency: %s, Position: %s, Avg Cost: %s\n", account.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), DecimalFunctions::decimalStringToDisplay(position).c_str(), Utils::doubleMaxString(avgCost).c_str());
	(*m_pPositions)[contract.conId] = Position(contract,stoi(DecimalFunctions::decimalStringToDisplay(position)),avgCost);
}

void Client::positionEnd() {
	printf( "PositionEnd\n");
}

void Client::reqFirstFut(){
	m_state = ST_ACK;
	m_pClient->reqContractDetails(ST_REQFIRSTFUT, MyContract::FUTURE("ESTX50","EUREX","FSXE","EUR"));
}

void Client::reqSlice(){
	printf("Requesting Slice");
	m_state = ST_ACK;
	m_pClient->reqContractDetails(ST_REQSLICE, MyContract::OPTION_SLICE( m_pSlice->forward.contract.symbol, m_pSlice->forward.contract.lastTradeDate, m_pSlice->forward.contract.exchange, m_pSlice->forward.contract.currency));
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void Client::contractDetails( int reqId, const ContractDetails& contractDetails){
	switch( reqId){
	case ST_REQFIRSTFUT:
		m_pSlice->assign_forward( contractDetails );
		break;
	case ST_REQSLICE:
		if(!m_pSlice->forward.expiry_str.size())
			throw std::runtime_error("Forward inccorectly set");
		m_pSlice->assign_option( contractDetails );
		break;
	}
    printContractMsg(contractDetails.contract);
}

void Client::contractDetailsEnd( int reqId) {
	printf( "Slice End. %d\n", reqId);
	if( reqId == ST_REQFIRSTFUT ){
		m_state = ST_REQSLICE;
	} else if( reqId == ST_REQSLICE ) {
		m_state = ST_REQMKTDATA;
	} else {
		m_state = ST_DISCONNECT;
	}
}

void Client::placeOrderFly( Option* l_opt,Option* m_opt,Option* h_opt){
	Contract fly = MyContract::FLY(l_opt->contract,m_opt->contract,h_opt->contract);
	long long qty = DecimalFunctions::doubleToDecimal(1); 
	Order order = MyOrder::COMBO_LIMIT_ORDER("BUY", qty, 0, false);
	m_pClient->placeOrder(m_orderId++,fly,order);	
}

void Client::sendSynth( bool long_side){
	string flag;
	float strike; 
	double limitPrice;
	const Forward& fwd = m_pSlice->forward;
	if( long_side){
		flag = "BUY";
		strike = *(m_pSlice->best_synth.second.begin());
		limitPrice = fwd.bid-strike;
	} else{
		flag = "SELL";
		strike = *(m_pSlice->best_synth.first.begin());
		limitPrice = fwd.ask-strike;
	}
	const Option& call = m_pSlice->options.at(strike).at(Option::CALL);
	const Option& put = m_pSlice->options.at(strike).at(Option::PUT);
	Contract synth = MyContract::SYNTH( m_pSlice->forward.contract, call.contract, put.contract );
	long long qty = DecimalFunctions::doubleToDecimal(1);
	Order order = MyOrder::COMBO_LIMIT_ORDER(flag, qty, limitPrice, false);
	m_pClient->placeOrder(m_orderId++,synth,order);
	int a = 1;	
}

void Client::reqMktData(){
	m_state = ST_ACK;
	m_pClient->reqMarketDataType(3);
	m_pClient->reqMktData(m_pSlice->forward.contract.conId, m_pSlice->forward.contract, "", false, false, TagValueListSPtr());
	m_pSlice->reqid_to_instrument[m_pSlice->forward.contract.conId] = &m_pSlice->forward;
	for(auto& opt:m_pSlice->options){
		for(auto& opt_ :opt.second){
			m_pClient->reqMktData(opt_.second.contract.conId, opt_.second.contract, "", false, false, TagValueListSPtr());
			m_pSlice->reqid_to_instrument[opt_.second.contract.conId] = &opt_.second;
		}
	}
}

void Client::tickSize( TickerId tickerId, TickType field, Decimal size){
	printf( "Tick Size. Ticker Id: %ld, Field: %d, Size: %s\n", tickerId, (int)field, DecimalFunctions::decimalStringToDisplay(size).c_str());
}
void Client::tickString(TickerId tickerId, TickType tickType, const std::string& value){
	printf( "Tick String. Ticker Id: %ld, Type: %d, Value: %s\n", tickerId, (int)tickType, value.c_str());
}
void Client::tickPrice( TickerId tickerId, TickType field, double price, const TickAttrib& attribs) {
    printf( "Tick Price. Ticker Id: %ld, Field: %d, Price: %s, CanAutoExecute: %d, PastLimit: %d, PreOpen: %d\n", tickerId, (int)field, Utils::doubleMaxString(price).c_str(), attribs.canAutoExecute, attribs.pastLimit, attribs.preOpen);
	Forward* instrument = m_pSlice->reqid_to_instrument[tickerId];
	m_pSlice->update_float_memb(instrument,field,price);	
}
void Client::tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions){
    printf("tickerId: %d, minTick: %s, bboExchange: %s, snapshotPermissions: %u\n", tickerId, Utils::doubleMaxString(minTick).c_str(), bboExchange.c_str(), snapshotPermissions);
	m_bboExchange = bboExchange;
}

void Client::error(int id, int errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson){
    if (!advancedOrderRejectJson.empty()) {
        printf("Error. Id: %d, Code: %d, Msg: %s, AdvancedOrderRejectJson: %s\n", id, errorCode, errorString.c_str(), advancedOrderRejectJson.c_str());
    } else {
        printf("Error. Id: %d, Code: %d, Msg: %s\n", id, errorCode, errorString.c_str());
    }
}

void Client::printContractMsg(const Contract& contract){
	printf("\tConId: %ld\n", contract.conId);
	printf("\tSymbol: %s\n", contract.symbol.c_str());
	printf("\tSecType: %s\n", contract.secType.c_str());
	printf("\tLastTradeDateOrContractMonth: %s\n", contract.lastTradeDateOrContractMonth.c_str());
	printf("\tLastTradeDate: %s\n", contract.lastTradeDate.c_str());
	printf("\tStrike: %s\n", Utils::doubleMaxString(contract.strike).c_str());
	printf("\tRight: %s\n", contract.right.c_str());
	printf("\tMultiplier: %s\n", contract.multiplier.c_str());
	printf("\tExchange: %s\n", contract.exchange.c_str());
	printf("\tPrimaryExchange: %s\n", contract.primaryExchange.c_str());
	printf("\tCurrency: %s\n", contract.currency.c_str());
	printf("\tLocalSymbol: %s\n", contract.localSymbol.c_str());
	printf("\tTradingClass: %s\n\n", contract.tradingClass.c_str());
}

void Client::orderStatus(OrderId orderId, const std::string& status, Decimal filled,
	Decimal remaining, double avgFillPrice, int permId, int parentId,
	double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice){
	printf("OrderStatus. Id: %ld, Status: %s, Filled: %s, Remaining: %s, AvgFillPrice: %s, PermId: %s, LastFillPrice: %s, ClientId: %s, WhyHeld: %s, MktCapPrice: %s\n", 
		orderId, status.c_str(), DecimalFunctions::decimalStringToDisplay(filled).c_str(), DecimalFunctions::decimalStringToDisplay(remaining).c_str(), Utils::doubleMaxString(avgFillPrice).c_str(), Utils::intMaxString(permId).c_str(),
		Utils::doubleMaxString(lastFillPrice).c_str(), Utils::intMaxString(clientId).c_str(), whyHeld.c_str(), Utils::doubleMaxString(mktCapPrice).c_str());
	(*m_pOrders)[orderId].status = status;
	(*m_pOrders)[orderId].filled = filled;
	(*m_pOrders)[orderId].remaining = remaining;
	(*m_pOrders)[orderId].avgFillPrice = avgFillPrice;
	(*m_pOrders)[orderId].permId = permId;
	(*m_pOrders)[orderId].parentId = parentId;
	(*m_pOrders)[orderId].lastFillPrice = lastFillPrice;
	(*m_pOrders)[orderId].clientId = clientId;
	(*m_pOrders)[orderId].whyHeld = whyHeld;
	(*m_pOrders)[orderId].mktCapPrice = mktCapPrice;
}

void Client::openOrder( OrderId orderId, const Contract& contract, const Order& order, const OrderState& orderState) {
    printf( "OpenOrder. PermId: %s, ClientId: %s, OrderId: %s, Account: %s, Symbol: %s, SecType: %s, Exchange: %s:, Action: %s, OrderType:%s, TotalQty: %s, CashQty: %s, "
        "LmtPrice: %s, AuxPrice: %s, Status: %s, MinTradeQty: %s, MinCompeteSize: %s, CompeteAgainstBestOffset: %s, MidOffsetAtWhole: %s, MidOffsetAtHalf: %s, " 
        "FAGroup: %s, FAMethod: %s, CustomerAccount: %s, ProfessionalCustomer: %s, BondAccruedInterest: %s\n",
        Utils::intMaxString(order.permId).c_str(), Utils::longMaxString(order.clientId).c_str(), Utils::longMaxString(orderId).c_str(), order.account.c_str(), contract.symbol.c_str(), 
        contract.secType.c_str(), contract.exchange.c_str(), order.action.c_str(), order.orderType.c_str(), DecimalFunctions::decimalStringToDisplay(order.totalQuantity).c_str(),
        Utils::doubleMaxString(order.cashQty).c_str(), Utils::doubleMaxString(order.lmtPrice).c_str(), Utils::doubleMaxString(order.auxPrice).c_str(), orderState.status.c_str(),
        Utils::intMaxString(order.minTradeQty).c_str(), Utils::intMaxString(order.minCompeteSize).c_str(), 
        order.competeAgainstBestOffset == COMPETE_AGAINST_BEST_OFFSET_UP_TO_MID ? "UpToMid" : Utils::doubleMaxString(order.competeAgainstBestOffset).c_str(),
        Utils::doubleMaxString(order.midOffsetAtWhole).c_str(), Utils::doubleMaxString(order.midOffsetAtHalf).c_str(),
        order.faGroup.c_str(), order.faMethod.c_str(), order.customerAccount.c_str(), (order.professionalCustomer ? "true" : "false"), order.bondAccruedInterest.c_str());
	(*m_pOrders)[orderId].contract = contract;
	(*m_pOrders)[orderId].order = order;
	(*m_pOrders)[orderId].orderState = orderState;
}

void Client::openOrderEnd() {
	printf( "OpenOrderEnd\n");
}
