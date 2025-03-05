#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <sys/ioctl.h>
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
#define TIMEOUT -1


inline int setupSocket(int domain, struct sockaddr *addr, socklen_t addrlen) {
   int sockfd, rc, on = 1;
    
    sockfd = socket(domain, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log(ERROR, "Error creating socket");
        return -1;
    }

    rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(sockfd);
        return -1;
    }

    rc = ioctl(sockfd, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("ioctl() failed");
        close(sockfd);
        return -1
    }

    if (bind(sockfd, addr, addrlen) < 0) {
        log(ERROR, "Error binding socket");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, BACKLOG) < 0) {
        log(ERROR, "Error listening on socket");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int initIpv4() {
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    return setupSocket(AF_INET, (struct sockaddr *)&addr, sizeof(addr));
}

int initIpv6() {
    struct sockaddr_in6 addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(PORT);

    return setupSocket(AF_INET6, (struct sockaddr *)&addr, sizeof(addr));
}

int pollClients(pollfd *fds, int nfds) {
    log(DEBUG, "Polling for incoming connections ...");
    int poll_count = poll(fds, nfds, TIMEOUT);
    log(DEBUG, std::to_string(poll_count) + " incoming connections from poll");
    if (poll_count == -1) {
        log(ERROR, "Polling error");
        return -1;
    } else if (poll_count == 0) {
        log(INFO, "Poll timeout, no incoming connections");
        return 0;
    }
    return poll_count;
}

int acceptClients(int listen_sd, std::vector<pollfd> &clients) {
    int new_sd;
    log(DEBUG, "Accepting incoming connections ...");
    while (true) {
        new_sd = accept(listen_sd, NULL, NULL);
        if (new_sd < 0) {
            if (errno != EWOULDBLOCK) {
                log(ERROR, "Failed to accept client connection");
                return -1;
            }
            log(DEBUG, "All incoming connections accepted");
            return 0; // no more clients to accept
        }
        log(INFO, "Accepted connection - " + std::to_string(new_sd));
        clients.emplace_back(pollfd{new_sd, POLLIN, 0});
    }
    return new_sd;
}

int handleConnection(pollfd &sd, char *buffer) {
    int len, rc;
    while (true) {
        rc = recv(sd.fd, buffer, sizeof(buffer), 0);
        if (rc < 0) {
            if (errno != EWOULDBLOCK) {
                perror("recv() failed");
                return -1;
            }
            break;
        }
        if (rc == 0) {
            log(INFO, "Connection closed");
            return -1;
        }
        len = rc;
        log(INFO, std::to_string(len) + " bytes received");
        rc = send(sd.fd, buffer, len, 0);
        if (rc < 0) {
            perror("send() failed");
            return -1;
        }
    }
    return 0;
}

#endif // SOCKETS_H