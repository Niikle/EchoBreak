#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define PORT 6969
#define REC_PORT 6868
#define BUFFER_SIZE 256
#define IP "172.17.213.255"

void receive(int sock, sockaddr_in addr){
    std::cout << "\nstart listening\n";
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    socklen_t addr_len = sizeof(addr);
    while(true){
        ssize_t bytes_received = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &addr_len);
        if(bytes_received > 0){
            std::cout << buffer << std::endl;
            std::cout << "received some data\n";
        }
        memset(buffer, 0, sizeof(buffer));
    }
}

int main() {
    int sock;
    int opt = 1;
    struct sockaddr_in broadcast_addr;

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

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr(IP); 

    //receiving
    int rec_sock;
    struct sockaddr_in broadcast_addr_rec;
    if ((rec_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "receving socket error";
        close(rec_sock);
        return -1;
    }

    memset(&broadcast_addr_rec, 0, sizeof(broadcast_addr_rec));
    broadcast_addr_rec.sin_family = AF_INET;
    broadcast_addr_rec.sin_addr.s_addr = INADDR_ANY;
    broadcast_addr_rec.sin_port = htons(REC_PORT);

    if (bind(rec_sock, (struct sockaddr*)&broadcast_addr_rec, sizeof(broadcast_addr_rec)) < 0) {
        std::cout << "receiving binding err";
        close(rec_sock);
        return -1;
    }
    std::string message;
    std::cout << "Enter the message in format(hostname;cmd/func;command) to exit write exit: ";
    socklen_t addr_len = sizeof(broadcast_addr_rec);

    std::thread rec(receive, rec_sock, broadcast_addr_rec);

    while(true){
        //sending
        std::getline(std::cin, message);
        if(message == "exit"){
            break;
        }
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
        message = "";
    }
    rec.join();
    close(sock);
    return 0;
}
