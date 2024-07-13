#pragma once
#ifndef MY_LOG
#define MY_LOG

#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>

class MyLog
{
    public:
        MyLog(std::string name = "log") {
            log_file.open("log/"+name,std::ofstream::app);
            //check_log();
        }
        ~MyLog(){log_file.close();}
        template<typename T> void write(const char* message,T value){
            //check_log();
            std::time_t now = std::time(nullptr);
            std::tm* localTime = std::localtime(&now);
            log_file << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << ": "  << message  << " " << value << std::endl;
        }
        void check_log(){
            if(!log_file.good()){
                throw std::runtime_error("Log file in bad state!");
            }
        }
    private:
        std::ofstream log_file;
};
#endif