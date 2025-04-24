#include <cstdio>
#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 6969
#define BUFFER_SIZE 256

int main() {
    int sock, response;
    int opt = 1;
    struct sockaddr_in broadcastAddr, respAddr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    if ((response = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "resp socket creation error" << std::endl;
        return -1;
    }

    //enable broadcast
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        close(sock);
        return -1;
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(PORT);
    broadcastAddr.sin_addr.s_addr = inet_addr("172.17.213.255"); 

    memset(&respAddr, 0, sizeof(respAddr));
    respAddr.sin_family = AF_INET;
    respAddr.sin_port = htons(PORT);
    respAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(response, (struct sockaddr *)&respAddr, sizeof(respAddr)) < 0) {
        std::cerr << "bind failed\n";
        close(response);
    }

    std::string message;
    std::cout << "Enter the message in format(hostname;cmd/func;command): ";
    
    char buffer[BUFFER_SIZE];
    ssize_t bytesReceived;

    socklen_t addrLen = sizeof(respAddr);

    while(true){
        //sending
        std::getline(std::cin, message);
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

        bytesReceived = recvfrom(response, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&respAddr, &addrLen);
        if(bytesReceived > 0){
            std::cout << buffer;
            memset(buffer, 0, sizeof(buffer));
        }
    }
    close(sock);
    return 0;
}