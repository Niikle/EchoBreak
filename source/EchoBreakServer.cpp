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
    int sock;
    int opt = 1;
    struct sockaddr_in broadcastAddr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
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

    std::string message;
    std::cout << "Enter the message in format(hostname;cmd/func;command): ";
    
    char buffer[BUFFER_SIZE];
    ssize_t bytesReceived;

    socklen_t addrLen = sizeof(broadcastAddr);

    while(true){
        //sending
        std::getline(std::cin, message);
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

        bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&broadcastAddr, &addrLen);
        if(bytesReceived > 0){
            std::cout << buffer;
            memset(buffer, 0, sizeof(buffer));
        }
    }
    close(sock);
    return 0;
}