#pragma once
#ifndef SLICE_H
#define SLICE_H 

#include <string>
#include "Contract.h"

struct Forward{
    Contract contract;
    float bid;
    float offer;
    float last;
    std::string expiry;
};

struct Slice{
    Forward forward;
};

#endif

