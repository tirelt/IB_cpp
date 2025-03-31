#pragma once
#ifndef MY_ORDER_H
#define MY_ORDER_H

#include <string>
#include "Order.h"

struct Contract;

class MyOrder {
public:
    static Order COMBO_LIMIT_ORDER(std::string action, Decimal quantity, double limitPrice, bool nonGuaranteed);
    static Order MARKET_ORDER(std::string action, Decimal quantity);
    static Order LIMIT_ORDER(std::string action, Decimal quantity, double limitPrice);
};

#endif
