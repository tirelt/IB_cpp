#pragma once
#ifndef SLICE_H
#define SLICE_H 

#include <string>
#include "Contract.h"
#include <map>

struct Forward{
    Contract contract;
    float bid;
    float offer;
    float last;
    std::string expiry;
    time_t last_update;  
};

struct Option{
    enum Right {PUT,CALL};
    Contract contract;
    float bid;
    float offer;
    float last;
    std::string expiry;
    float strike;
    Right right; 
    time_t last_update;    
};

struct Slice{
    Forward forward;
    std::map<float,std::map<Option::Right,Option>> options;
    void assign_forward(const Contract&);
    void assign_option(const Contract&);
};

#endif

