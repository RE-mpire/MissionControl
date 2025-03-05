#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

struct config {
    std::string host;
    int port;
    char mode;
};

void parse_args(int argc, char **argv, struct config *conf) {
    int opt;
    std::string host = "127.0.0.1";
    int port = 4000;
    char mode = 'c'; // default mode is controller

    while ((opt = getopt(argc, argv, "o:p:h:")) != -1) {
        switch (opt) {
            case 'o':
                mode = 'o';
                break;
            case 'p':
                port = std::stoi(optarg);
                break;
            case 'h':
                host = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-o] [-p port] [-h host]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    conf->host = host;
    conf->port = port;
    conf->mode = mode;
}

int main(int argc, char **argv) {
    struct config conf;
    parse_args(argc, argv, &conf);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error creating the socket ..." << std::endl;
        return 1;
    }

    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(conf.port);
    if (inet_pton(AF_INET, conf.host.c_str(), &clientAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return 1;
    }

    std::string mode = conf.mode == 'o' ? "Observer" : "Controller";

    std::cout << "Connecting to " << conf.host << ":" << conf.port << std::endl;

    int ret = connect(client_socket, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    if (ret == -1) {
        std::cerr << "Error connecting to server ..." << std::endl;
        return 1;
    }

    char mess[1024];
    while (true) {
        std::cout << mode << "> ";
        fgets(mess, sizeof(mess), stdin);
        send(client_socket, mess, strlen(mess), 0);
        if (strcmp(mess, "exit\n") == 0)
            break;
        memset(mess, 0, sizeof(mess));
    }
    close(client_socket);
    return 0;
}