#include <iostream>
#include <fstream>
#include <string>

class Logger {
    public:
        Logger(const std::string& fileName) : logFile(fileName, std::ios::app) {}
        ~Logger() { if (logFile.is_open()) logFile.close(); }

       template<typename T>
        Logger& operator<<(const T& value) {
            if(logFile.is_open())
                logFile << value; 
            logFile << std::endl;   
            return *this; 
        }
    private:
        std::ofstream logFile;
};