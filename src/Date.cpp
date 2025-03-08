#include "Date.h"
#include <ctime>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <iostream>

using std::tm;
using std::string;
using std::time_t;
using std::mktime;
using std::localtime;
using std::time;
using std::istringstream;
using std::get_time;

time_t get_last_sunday_of_month(int year, int month, int last_day){
    tm lastSunday = {};
    lastSunday.tm_year = year - 1900; // Year since 1900
    lastSunday.tm_mon = month - 1;            // March (0-based index, so 2 is March)
    lastSunday.tm_mday = last_day;          // Start at the last day of March
    time_t lastSundayTime = mktime(&lastSunday);
    tm* timeInfo = localtime(&lastSundayTime);
    int w_day = timeInfo->tm_wday;
    lastSunday.tm_mday -= w_day;      
    return mktime(&lastSunday);
}

time_t Date::create_from_string(const string time_str, const string date_str,const string exchange){
    string dateTimeStr = date_str + " " + time_str;
    tm time_struct = {};
    istringstream ss(dateTimeStr);
    ss >> get_time(&time_struct, "%Y%m%d %H:%M:%S");
    time_t timeT = mktime(&time_struct);
    int shift = 0;
    if(exchange=="EUREX"){
        shift = 1;
        if( timeT > get_last_sunday_of_month( time_struct.tm_year + 1900, 3, 31 ) && timeT < get_last_sunday_of_month( time_struct.tm_year + 1900, 10, 31 ) )
            ++shift;
    } else {
        throw std::runtime_error( "exchange not recognized" );
    }
    timeT += shift * 60 * 60;
    tm* localTime = std::localtime(&timeT);
    std::cout << "Current date and time: "<< std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << std::endl;
    return timeT;
}