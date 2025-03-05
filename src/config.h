#ifndef CONFIG_H
#define CONFIG_H

#include "device.h"
#include <iostream>
#include <cstring>


struct Config
{
    std::string logFile;
    LogLevel minLogLevel;
    std::vector<Device> devices;
};


void printHelp() {
    std::cout << "Usage: program [options]\n"
              << "Options:\n"
              << "  --help          Show this help message\n"
              << "  --flag1         Description for flag1\n"
              << "  --flag2         Description for flag2\n";
}

void parseArgs(int argc, char **argv, Config &config) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            printHelp();
            exit(0);
        } else if (strcmp(argv[i], "--flag1") == 0) {
            // Handle flag1
        } else if (strcmp(argv[i], "--flag2") == 0) {
            // Handle flag2
        } else {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            printHelp();
            exit(1);
        }
    }
}

#endif  // CONFIG_H