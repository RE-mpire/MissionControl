#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <vector>

#include "logging.h"

#define PORT 4000
#define BACKLOG 5
#define TIMEOUT 5000


int initIpv4() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log(ERROR, "Error creating IPv4 socket");
        return -1;
    }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        log(ERROR, "Error binding IPv4 socket");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, BACKLOG) < 0) {
        log(ERROR, "Error listening on IPv4 socket");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int initIpv6() {
    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log(ERROR, "Error creating IPv6 socket");
        return -1;
    }

    struct sockaddr_in6 addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        log(ERROR, "Error binding IPv6 socket");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, BACKLOG) < 0) {
        log(ERROR, "Error listening on IPv6 socket");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int pollConnections(std::vector<int> &fds) { // TODO: reimplemenet using poll()
    return 0;
}

#endif // SOCKETS_H