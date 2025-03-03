#pragma once
#ifndef SLICE_H
#define SLICE_H 

#include <string>
#include "Contract.h"
#include <map>

struct Forward{
    enum Field {BID};
    enum Right {PUT,CALL,FORWARD};
    Contract contract;
    Right right; 
    double bid;
    double ask;
    double last;
    std::string expiry;
    time_t last_update;  
    double time_to_maturity;
    void work_after_update( const double& fwd_price, double Forward::* memb ){}
};

struct Option : public Forward{
    float strike;
    double vol_bid;
    double vol_ask;
    void work_after_update( const double& fwd_price, double Forward::* memb );
};

struct Slice{
    Forward forward;
    std::map<float,std::map<Option::Right,Option>> options;
    std::map<int,Forward *> reqid_to_instrument;
    void assign_forward(const Contract&);
    void assign_option(const Contract&);
    void update_float_memb( Forward * fwd,const int field,const double value);
};

#endif

