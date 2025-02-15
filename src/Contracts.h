#pragma once
#ifndef TWS_API_SAMPLES_TESTCPPCLIENT_CONTRACTSAMPLES_H
#define TWS_API_SAMPLES_TESTCPPCLIENT_CONTRACTSAMPLES_H

#include <string>

struct Contract;

class MyContract {
public:
	static Contract OPTION_SLICE(std::string trading_class, std::string expiration);
};

#endif
