#pragma once
#ifndef DATE_H
#define DATE_H

#include <string>
#include <ctime>

struct Date{
    static std::time_t create_from_string( const std::string time_str, const std::string date_str, const std::string exchange);
};

#endif
