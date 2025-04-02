#pragma once
#ifndef SLICE_H
#define SLICE_H 

#include <string>
#include "Contract.h"
#include <map>
#include "TaskQueue.h"
#include <utility>
#include <fstream>
#include "Log.h"
#include "Client.h"

class Client;

struct Forward{
    enum Field {BID};
    enum Right {PUT,CALL,FORWARD};
    Contract contract;
    Right right; 
    double bid = -1;
    double ask = -1;
    double last = -1;
    time_t expiry;
    std::string expiry_str;
    time_t last_update;  
    double time_to_maturity;
    virtual void work_after_update( const double& fwd_price, double Forward::* memb ){}
};

struct Option : public Forward{
    float strike;
    double vol_bid;
    double vol_ask;
    void work_after_update( const double& fwd_price, double Forward::* memb ) override;
};
struct Synthetic{
    double price=-1;
    Option* call;
    Option* put;
    double forward_price = -1;
};

struct Slice{
    Slice();
    ~Slice();
    TaskQueue* imply_vol_queue;
    std::thread imply_vol_t;
    std::string contractMonth; 
    Forward forward;
    std::map<float,std::map<Option::Right,Option>> options;
    std::map<int,Forward *> reqid_to_instrument;
    void assign_forward(const ContractDetails&);
    void assign_option(const ContractDetails&);
    void update_float_memb( Forward * fwd,const int field,const double value);
    std::map<float,std::pair<float,float>> synth_prices;
    std::set<float> updated_strikes;
    std::pair<float,float> best_synth_prices;
    std::pair<std::set<float>,std::set<float>> best_synth;
    void update_synthetic();
    void check_fly();
    void check_fly_aux(Option* l_opt,Option* m_opt,Option* h_opt);
    Logger log;
    Client* m_pClient;
};

#endif

