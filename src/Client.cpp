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

Client::Client() :
      m_osSignal(2000)//2-seconds timeout
    , m_pClient(new EClientSocket(this, &m_osSignal))
	, m_state(ST_CONNECT)	
	, m_sleepDeadline(0)
	, m_orderId(0)
    , m_extraAuth(false)
	, m_pSlice(new Slice){
}
//! [socket_init]
Client::~Client(){
	// destroy the reader before the client
	if( m_pReader )
		m_pReader.reset();
	if( m_pSlice )
		m_pSlice.reset();
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
			m_state = ST_REQSLICE;
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
		case ST_ACK:
			//const unsigned sleep(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			printf("\n\ncycle ACK\n\n");
			break;
	}

	m_osSignal.waitForSignal();
	errno = 0;
	m_pReader->processMsgs();
}

void Client::connectAck(){
	if (!m_extraAuth && m_pClient->asyncEConnect())
        m_pClient->startApi();
}

void Client::reqPositions(){
	printf("Requesting Positions");
	m_pClient->reqPositions();
}

void Client::position( const std::string& account, const Contract& contract, Decimal position, double avgCost) {
    printf( "Position. %s - Symbol: %s, SecType: %s, Currency: %s, Position: %s, Avg Cost: %s\n", account.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), DecimalFunctions::decimalStringToDisplay(position).c_str(), Utils::doubleMaxString(avgCost).c_str());
}

void Client::positionEnd() {
	printf( "PositionEnd\n");
	m_pClient->cancelPositions();
}

void Client::reqFirstFut(){
	m_state = ST_ACK;
	m_pClient->reqContractDetails(ST_REQFIRSTFUT, MyContract::FUTURE("ESTX50","EUREX","EUR"));
}

void Client::reqSlice(){
	printf("Requesting Slice");
	m_state = ST_ACK;
	m_pClient->reqContractDetails(ST_REQSLICE, MyContract::OPTION_SLICE( m_pSlice->forward.contract.symbol, m_pSlice->forward.contract.lastTradeDate, m_pSlice->forward.contract.exchange, m_pSlice->forward.contract.currency));
}

void Client::contractDetails( int reqId, const ContractDetails& contractDetails){
	switch( reqId){
	case ST_REQFIRSTFUT:
		m_pSlice->assign_forward( contractDetails.contract);
		break;
	case ST_REQSLICE:
		m_pSlice->assign_option( contractDetails.contract);
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

void Client::reqMktData(){
	m_state = ST_ACK;
	m_pClient->reqMarketDataType(3);
	m_pClient->reqMktData(100, m_pSlice->forward.contract, "", false, false, TagValueListSPtr());
	m_pClient->reqMktData(101, m_pSlice->options[6000.][Option::CALL].contract, "", false, false, TagValueListSPtr());
	//std::this_thread::sleep_for(std::chrono::seconds(10));
	//m_pClient->cancelMktData(100);
}

void Client::tickSize( TickerId tickerId, TickType field, Decimal size){
	printf( "Tick Size. Ticker Id: %ld, Field: %d, Size: %s\n", tickerId, (int)field, DecimalFunctions::decimalStringToDisplay(size).c_str());
}
void Client::tickString(TickerId tickerId, TickType tickType, const std::string& value){
	printf( "Tick String. Ticker Id: %ld, Type: %d, Value: %s\n", tickerId, (int)tickType, value.c_str());
}
void Client::tickPrice( TickerId tickerId, TickType field, double price, const TickAttrib& attribs) {
    printf( "Tick Price. Ticker Id: %ld, Field: %d, Price: %s, CanAutoExecute: %d, PastLimit: %d, PreOpen: %d\n", tickerId, (int)field, Utils::doubleMaxString(price).c_str(), attribs.canAutoExecute, attribs.pastLimit, attribs.preOpen);
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

void Client::nextValidId( OrderId orderId){
	printf("Next Valid Id: %ld\n", orderId);
	m_orderId = orderId;
	//! [nextvalidid]
	m_state = ST_REQFIRSTFUT;

    //m_state = ST_TICKOPTIONCOMPUTATIONOPERATION; 
    //m_state = ST_TICKDATAOPERATION; 
    //m_state = ST_OPTIONSOPERATIONS;
    //m_state = ST_REQTICKBYTICKDATA; 
    //m_state = ST_REQHISTORICALTICKS; 
    //m_state = ST_CONTFUT; 
    //m_state = ST_PNLSINGLE; 
    //m_state = ST_PNL; 
	//m_state = ST_DELAYEDTICKDATAOPERATION; 
	//m_state = ST_MARKETDEPTHOPERATION;
	//m_state = ST_REALTIMEBARS;
	//m_state = ST_MARKETDATATYPE;
	//m_state = ST_HISTORICALDATAREQUESTS;
	//m_state = ST_CONTRACTOPERATION;
	//m_state = ST_MARKETSCANNERS;
	//m_state = ST_FUNDAMENTALS;
	//m_state = ST_BULLETINS;
	//m_state = ST_ACCOUNTOPERATIONS;
	//m_state = ST_ORDEROPERATIONS;
	//m_state = ST_OCASAMPLES;
	//m_state = ST_CONDITIONSAMPLES;
	//m_state = ST_BRACKETSAMPLES;
	//m_state = ST_HEDGESAMPLES;
	//m_state = ST_TESTALGOSAMPLES;
	//m_state = ST_FAORDERSAMPLES;
	//m_state = ST_FAOPERATIONS;
	//m_state = ST_DISPLAYGROUPS;
	//m_state = ST_MISCELANEOUS;
	//m_state = ST_FAMILYCODES;
	//m_state = ST_SYMBOLSAMPLES;
	//m_state = ST_REQMKTDEPTHEXCHANGES;
	//m_state = ST_REQNEWSTICKS;
	//m_state = ST_REQSMARTCOMPONENTS;
	//m_state = ST_NEWSPROVIDERS;
	//m_state = ST_REQNEWSARTICLE;
	//m_state = ST_REQHISTORICALNEWS;
	//m_state = ST_REQHEADTIMESTAMP;
	//m_state = ST_REQHISTOGRAMDATA;
	//m_state = ST_REROUTECFD;
	//m_state = ST_MARKETRULE;
	//m_state = ST_PING;
	//m_state = ST_WHATIFSAMPLES;
	//m_state = ST_WSH;
	//m_state = ST_RFQOPERATIONS;
}
