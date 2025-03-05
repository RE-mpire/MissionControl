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
    // initDevices(config.devices);

    std::vector<struct pollfd> clients;
    clients.reserve(200);
    
    bool compaction = false;
    int rc, len, new_sd = -1;
    int listen_sd = initIpv6();
    clients[0].fd = listen_sd;
    clients[0].events = POLLIN;

    auto t1 = processorThread(); // list of commands to process (process command and update client when complete)
    auto t2 = streamThread(); // stream data back, process subscribe requests (device polling loop here)

    while (running) {
        if (pollClients(clients.data(), clients.size()) < 0) { 
            running = false;
        }

        for (auto &sd : clients) {
            if (sd.revents == 0) continue;
            if (sd.revents != POLLIN) {
                log(ERROR, "Error! revents = " + std::to_string(sd.revents));
            }
            if (sd.fd == listen_sd) {
                if (acceptClients(listen_sd, clients) < 0) {
                    running = false;
                    break;
                }
            } else {
                char buffer[1024];
                rc = handleConnection(sd, buffer);
                if (rc < 0) {
                    close(sd.fd);
                    sd.fd = -1;
                    compaction = true;
                } else if (rc == 1) {
                    dispatch(t1, buffer);
                } else if (rc == 2) {
                    dispatch(t2, buffer);
                }
            }
        }
        if (compaction) {
            compaction = false;
            clients.erase(std::remove_if(clients.begin(), clients.end(), [](const pollfd &pfd) {
                return pfd.fd == -1;
            }), clients.end());
        }
    }
    log(INFO, "Server shutting down ...");
    t1.shutdown(); // notify all threads to shutdown and notify clients
    t2.shutdown();
    // shutdownDevices(config.devices);

    for (auto &sd : clients) {
        close(sd.fd);
    }
    log(INFO, "Server shutdown successfully");
    return 0;
}
