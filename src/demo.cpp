#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>

#include "logging.h"
#include "config.h"
#include "device.h"
#include "sockets.h"

volatile sig_atomic_t running = 1;

void handleSignal(int signal) {
    if (signal == SIGINT) {
        running = 0;
    }
}

void setupSignalHandler() {
    struct sigaction sa;
    sa.sa_handler = handleSignal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, nullptr);
}

int main(int argc, char **argv) {
    Config config;
    setupSignalHandler();
    parseArgs(argc, argv, config);
    initLogger(config.logFile, config.minLogLevel);
    initDevices(config.devices);

    std::array<int, 2> sockets;
    std::vector<int> clients;
    clients.reserve(100);

    sockets[0] = initIpv6();
    sockets[1] = initIpv4();

    auto t1 = processorThread(); // list of commands to process (process command and update client when complete)
    auto t2 = streamThread(); // stream data back, process subscribe requests (device polling loop here)

    while (running) {
        // poll for connections to accept
        int fd = pollConnections(sockets, clients);
        // accept connection if not at limit (close listener?)
        if (fd < sockets.size()) { 
            acceptConnection(fd);
        } else {
            handleConnection(fd); // forward data to proper thread
        }
    }
    log(INFO, "Server shutting down ...");
    t1.shutdown(); // notify all threads to shutdown and notify clients
    t2.shutdown();
    shutdownDevices(config.devices);

    for (auto &fd : sockets) {
        close(fd);
    }
    log(INFO, "Server shutdown successfully");
    return 0;
}
