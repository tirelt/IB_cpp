#pragma once
#ifndef MY_ORDER_H
#define MY_ORDER_H

#include <string>
#include "Order.h"
#include "CommonDefs.h"
#include "Contract.h"
#include "OrderState.h"

struct Contract;

class MyOrder {
public:
    static Order COMBO_LIMIT_ORDER(std::string action, Decimal quantity, double limitPrice, bool nonGuaranteed);
    static Order MARKET_ORDER(std::string action, Decimal quantity);
    static Order LIMIT_ORDER(std::string action, Decimal quantity, double limitPrice);
};

struct LiveOrder{
    OrderId orderId; 
    Contract contract;
    Order order;
    OrderState orderState;
    std::string status;
    Decimal filled;
	Decimal remaining;
    double avgFillPrice;
    int permId;
    int parentId;
	double lastFillPrice;
    int clientId;
    std::string whyHeld; 
    double mktCapPrice;
};

#endif
