#pragma once
#ifndef POSITION_H
#define POSITION_H

#include <string>
#include <map>
#include "TaskQueue.h"
#include "Contract.h"

struct Position{
    Position(){}
    Position(const Contract& c,const int& q, const double& cost):contract(c),qty(q),avgCost(cost){}
    Contract contract;
    int qty;
    double avgCost;
};

#endif
