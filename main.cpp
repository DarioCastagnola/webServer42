#include "TcpServer.hpp"

int main(int argc, char **argv) {
    if (argc == 2)
    {
        TcpServer server("0.0.0.0", 8080, argv[1]);
        server.startListen();
    }
    return 1;
}