#pragma once
#ifndef TWS_API_SAMPLES_TESTCPPCLIENT_CONTRACTSAMPLES_H
#define TWS_API_SAMPLES_TESTCPPCLIENT_CONTRACTSAMPLES_H

#include <string>

struct Contract;

class MyContract {
public:
	static Contract OPTION_SLICE(std::string symbol, std::string expiration, std::string exchange, std::string currency);
    static Contract OptionWithTradingClass();
};

#endif
