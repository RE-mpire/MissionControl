#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <sstream>
#include <ctime>
#include <iomanip>

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    Logger(const std::string& filename, LogLevel minLogLevel = DEBUG) 
        : logFile(filename, std::ios::app), minLogLevel(minLogLevel) {
        if (!logFile.is_open()) {
            throw std::runtime_error("Unable to open log file");
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(LogLevel level, const std::string& message) {
        if (level < minLogLevel) {
            return;
        }
        std::lock_guard<std::mutex> guard(logMutex);
        logFile << "[" << getCurrentTime() << "] [" << getLabel(level) << "] " << message << std::endl;
    }

private:
    std::ofstream logFile;
    std::mutex logMutex;
    LogLevel minLogLevel;

    std::string getLabel(LogLevel level) {
        switch (level) {
            case DEBUG: return "DEBUG";
            case INFO: return "INFO";
            case WARNING: return "WARNING";
            case ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
        return ss.str();
    }
};


static Logger* logger = nullptr;

void initLogger(const std::string& filename, LogLevel minLogLevel = DEBUG) {
    static std::once_flag flag;
    std::call_once(flag, [&]() {
        logger = new Logger(filename, minLogLevel);
    });
}

void log(LogLevel level, const std::string& message) {
   logger->log(level, message);
}

#endif // LOGGING_H