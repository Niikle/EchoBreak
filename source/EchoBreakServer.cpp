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
#define REC_PORT 6868
#define BUFFER_SIZE 256

int main() {
    int sock;
    int opt = 1;
    struct sockaddr_in broadcastAddr;

    // sending
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

    //receiving
    int rec_sock;
    struct sockaddr_in broadcastAddr_rec;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    if ((rec_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "receving socket error";
        close(rec_sock);
        return -1;
    }

    memset(&broadcastAddr_rec, 0, sizeof(broadcastAddr_rec));
    broadcastAddr_rec.sin_family = AF_INET;
    broadcastAddr_rec.sin_addr.s_addr = INADDR_ANY;
    broadcastAddr_rec.sin_port = htons(REC_PORT);

    if (bind(rec_sock, (struct sockaddr*)&broadcastAddr_rec, sizeof(broadcastAddr_rec)) < 0) {
        std::cout << "receiving binding err";
        close(rec_sock);
        return -1;
    }


    std::string message;
    std::cout << "Enter the message in format(hostname;cmd/func;command): ";
    socklen_t addrLen = sizeof(broadcastAddr_rec);
    while(true){
        //sending
        std::getline(std::cin, message);
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        //receiving
        ssize_t bytesReceived = recvfrom(rec_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&broadcastAddr_rec, &addrLen);
        if(bytesReceived > 0){
            std::cout << buffer << std::endl;
        }
        memset(buffer, 0, sizeof(buffer));
        message = "";
    }
    close(sock);
    return 0;
}
