#include "Date.h"
#include <ctime>
#include <string>
#include <stdexcept>

using std::tm;
using std::string;
using std::time_t;
using std::mktime;
using std::localtime;
using std::time;

tm get_last_sunday_of_month(int year, int month, int last_day){
    tm lastSunday = {};
    lastSunday.tm_year = year - 1900; // Year since 1900
    lastSunday.tm_mon = month;            // March (0-based index, so 2 is March)
    lastSunday.tm_mday = last_day;          // Start at the last day of March
    time_t lastSundayTime = mktime(&lastSunday);
    tm* timeInfo = localtime(&lastSundayTime);
    int w_day = timeInfo->tm_wday;
    lastSunday.tm_mday -= w_day;      
    return lastSunday;
}

time_t Date::create_from_string(const string time_str, const string date_str,const string exchange){
    if(exchange=="EUREX"){

    } else {
        throw std::runtime_error( "exchange not recognized" );
    }
    return time(nullptr);
}