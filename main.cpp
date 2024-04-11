#include "TcpServer.hpp"

int main(int argc, char **argv) {
    if (argc > 2)
    {
        std::cout << RED <<"Wrong usage" << RESET << std::endl;
        return -1;
    }
    std::string configFile;
    if (argc == 1) {
        configFile = "theConfigFile.conf";
    } else {
        configFile = argv[1];
    }
    TcpServer server("0.0.0.0", 8080, configFile);
    server.startListen();
    return 0;
}
