#pragma once
#ifndef DATE_H
#define DATE_H

#include <string>
#include <ctime>

struct Date{
    std::time_t create_from_string( const std::string , const std::string , const std::string );
};

#endif
