#pragma once
#ifndef SLICE_H
#define SLICE_H 

#include <string>
#include "Contract.h"
#include <map>

struct Forward{
    enum Field {BID};
    Contract contract;
    float bid;
    float ask;
    float last;
    std::string expiry;
    time_t last_update;  
    void update_float_memb(const float value, const int field);
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

