#include <iostream>
#include <fstream>
#include <string>

class Logger {
    public:
        Logger(const std::string& fileName) : logFile(fileName ) {}
        ~Logger() { if (logFile.is_open()) logFile.close(); }

       template<typename T>
        Logger& operator<<(const T& value) {
            if(logFile.is_open())
                logFile << value; 
            return *this; 
        }
        Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
            if (logFile.is_open())
                logFile << manip;
            return *this;
        }
    private:
        std::ofstream logFile;
};