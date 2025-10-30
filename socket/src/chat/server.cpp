//
// Created by mirla on 17/10/2025.
//
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>



void processClientMessage(int client){
    char buffer[1024];
    std::cout << "Thread " << std::this_thread::get_id() << " processing client messages." <<  std::endl;

    recv(client, buffer, sizeof(buffer), 0);
    std::cout << buffer;
}


int main() {    
    int fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    std::vector<std::thread> threads;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(8888);

    bind(fdSocket, (sockaddr *) &serverAddr, sizeof(serverAddr));
    listen(fdSocket, 10);

    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    std::cout << "Server waiting for clientes..." << std::endl;

    int clientSocket = accept(
        fdSocket,
        (sockaddr *) &clientAddr,
        &clientAddrLen);
    
    threads.emplace_back(
        processClientMessage,
        std::ref(clientSocket)
    );

}