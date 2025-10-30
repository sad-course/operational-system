//
// Created by mirla on 17/10/2025.
//
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"

int main() {
    int socketFd;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(8888);

    std::cout << "Connecting to server..." << std::endl;
    connect(socketFd, (sockaddr *) &serverAddr, sizeof(serverAddr));

    bool quitChat = false;
    std::string user;
    std::cin >> user;
    while (!quitChat) {
        chatMessage message;
        char text[1024];
        std::cout << "Enter command: ";
        std::cin >> text ;
        std::cout << std::flush;

        if (text == "5"){
            quitChat = true;
        }
        send(socketFd, text, sizeof(text),0);

    }

    close(socketFd);
}