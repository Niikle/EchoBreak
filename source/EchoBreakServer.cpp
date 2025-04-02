#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 6969
#define BUFFER_SIZE 256

int main() {
    int sock, response;
    struct sockaddr_in broadcastAddr, responseAddr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    //enable broadcast
    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        close(sock);
        return -1;
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(PORT);
    broadcastAddr.sin_addr.s_addr = inet_addr("172.17.213.255"); 

    bool rec = 0;

    if ((response = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        std::cout << "response sock failet. continue\n";
    }
    else{
        memset(&responseAddr, 0, sizeof(responseAddr));
        responseAddr.sin_family = AF_INET;
        responseAddr.sin_addr.s_addr = INADDR_ANY;
        responseAddr.sin_port = htons(PORT);
        if(bind(response, (struct sockaddr*)&responseAddr, sizeof(responseAddr)) < 0){
            std::cout << "bind failed. continue\n";
        }
        else rec = 1;
    }

    std::string message;
    std::cout << "Enter the message in format(hostname;cmd/func(later);command): ";
    
    char buffer[BUFFER_SIZE];
    socklen_t reclen = sizeof(responseAddr);
    ssize_t bytesReceived;
    while(true){
        //sending
        std::getline(std::cin, message);
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        //receiving
        bytesReceived = recvfrom(response, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&responseAddr, &reclen);
        if(bytesReceived > 0){
            std::cout << buffer << std::endl;
            memset(buffer, 0, sizeof(buffer));
        }
    }
    close(sock);
    return 0;
}