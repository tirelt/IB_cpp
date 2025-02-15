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

const int PING_DEADLINE = 2; // seconds
const int SLEEP_BETWEEN_PINGS = 30; // seconds

///////////////////////////////////////////////////////////
// member funcs
//! [socket_init]
Client::Client() :
      m_osSignal(2000)//2-seconds timeout
    , m_pClient(new EClientSocket(this, &m_osSignal))
	, m_state(ST_CONNECT)
	, m_sleepDeadline(0)
	, m_orderId(0)
    , m_extraAuth(false){
}
//! [socket_init]
Client::~Client(){
	// destroy the reader before the client
	if( m_pReader )
		m_pReader.reset();

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

void Client::disconnect() const
{
	m_pClient->eDisconnect();

	printf ( "Disconnected\n");
}

bool Client::isConnected() const
{
	return m_pClient->isConnected();
}

void Client::processMessages()
{
	time_t now = time(NULL);

	/*****************************************************************/
    /* Below are few quick-to-test examples on the IB API functions grouped by functionality. Uncomment the relevant methods. */
    /*****************************************************************/
	switch (m_state) {
		case ST_PING:
			reqCurrentTime();
			break;
		case ST_PING_ACK:
			if( m_sleepDeadline < now) {
				disconnect();
				return;
			}
			break;
		case ST_IDLE:
			if( m_sleepDeadline < now) {
				m_state = ST_PING;
				return;
			}
			break;
		case ST_REQPOSITIONS:
			reqPositions();
			m_state = ST_REQSLICE;
			break;
		case ST_REQSLICE:
			reqSlice();
			break;
		case ST_DISCONNECT:
			disconnect();
			return;
	}

	m_osSignal.waitForSignal();
	errno = 0;
	m_pReader->processMsgs();
}

//////////////////////////////////////////////////////////////////
// methods
//! [connectack]
void Client::connectAck(){
	if (!m_extraAuth && m_pClient->asyncEConnect())
        m_pClient->startApi();
}
//! [connectack]

void Client::reqCurrentTime(){
	printf( "Requesting Current Time\n");

	// set ping deadline to "now + n seconds"
	m_sleepDeadline = time( NULL) + PING_DEADLINE;

	m_state = ST_PING_ACK;

	m_pClient->reqCurrentTime();
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

void Client::reqSlice(){
	printf("Requesting Slice");
	m_pClient->reqContractDetails(1,MyContract::OPTION_SLICE("OESX","20250321"));
}

void Client::contractDetails( int reqId, const ContractDetails& contractDetails) {
    printContractMsg(contractDetails.contract);
}

void Client::contractDetailsEnd( int reqId) {
	printf( "Slice End. %d\n", reqId);
	m_state = ST_DISCONNECT;
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
	printf("\tTradingClass: %s\n", contract.tradingClass.c_str());
}

void Client::nextValidId( OrderId orderId){
	printf("Next Valid Id: %ld\n", orderId);
	m_orderId = orderId;
	//! [nextvalidid]
	m_state = ST_REQPOSITIONS;

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

void Client::currentTime( long time){
	if ( m_state == ST_PING_ACK) {
		time_t t = ( time_t)time;
        struct tm timeinfo;
        char currentTime[80];

#if defined(IB_WIN32)
        localtime_s(&timeinfo, &t);
        asctime_s(currentTime, sizeof currentTime, &timeinfo);
#else
        localtime_r(&t, &timeinfo);
        asctime_r(&timeinfo, currentTime);
#endif
        printf( "The current date/time is: %s", currentTime);

		time_t now = ::time(NULL);
		m_sleepDeadline = now + SLEEP_BETWEEN_PINGS;

		m_state = ST_PING_ACK;
	}
}
