#include "MyContract.h"
#include "Contract.h"

Contract ContractSamples::AAPL(){
	Contract contract;
	contract.symbol = "AAPL";
	contract.secType = "Stocks";
	contract.currency = "USD";
	contract.exchange = "NASDAQ";
	return contract;
}