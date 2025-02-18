#pragma once
#ifndef SLICE_H
#define SLICE_H 

#include <string>
#include "Contract.h"
#include <map>

struct Forward{
    enum Field {BID};
    Contract contract;
    double bid;
    double ask;
    double last;
    std::string expiry;
    time_t last_update;  
    void update_float_memb( const int field,const double value);
};

struct Option : public Forward{
    enum Right {PUT,CALL};
    float strike;
    Right right; 
};

struct Slice{
    Forward forward;
    std::map<float,std::map<Option::Right,Option>> options;
    std::map<int,Forward *> reqid_to_instrument;
    void assign_forward(const Contract&);
    void assign_option(const Contract&);
};

#endif

