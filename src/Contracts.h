#pragma once
#ifndef MY_CONTRACT_H
#define MY_CONTRACT_H

#include <string>

struct Contract;

class MyContract {
public:
	static Contract OPTION_SLICE(std::string symbol, std::string expiration, std::string exchange, std::string currency);
	static Contract FUTURE(std::string symbol, std::string exchange, std::string tradingClass, std::string currency);
	static Contract FLY(Contract& l_c,Contract& m_c, Contract& h_c);
};

#endif
