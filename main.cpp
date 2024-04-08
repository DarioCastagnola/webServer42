#include "TcpServer.hpp"

int main() {
    TcpServer server("0.0.0.0", 8080);
    server.startListen();
    return 1;
}