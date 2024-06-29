#pragma once
#ifndef MY_LOG
#define MY_LOG

#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>

class Log
{
    public:
        Log(std::string name = "log") {log_file.open("log/"+name,std::ofstream::app);};
        ~Log(){log_file.close();}
        template<typename T> void write(const char* message,T value){
            std::time_t now = std::time(nullptr);
            std::tm* localTime = std::localtime(&now);
            log_file << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << ": "  << message  << " " << value << std::endl;
        }
    private:
        std::ofstream log_file;
};
#endif