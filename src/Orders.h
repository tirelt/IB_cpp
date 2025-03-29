#pragma once
#ifndef MY_ORDER_H
#define MY_ORDER_H

#include <string>

struct Contract;

class MyOrder {
public:
    static Order COMBOLIMITORDER(std::string action, Decimal quantity, double limitPrice, bool nonGuaranteed);
};

#endif
