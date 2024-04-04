#include "TcpServer.hpp"

TcpServer::TcpServer(std::string ip_address, int port): _ip_address(ip_address), _port(port) {
    this->_socketAddress = NULL;
    this->_new_socket = 0;
    this->_socket = 0;
    this->_incoming_message = 0;
    this->_socketAddress_len = sizeof(this->_socketAddress);
    this->_socketAddress.sin_family = AF_INET;
    this->_socketAddress.sin_port = htons(this->_port);
    this->_socketAddress.sin_addr.s_addr = inet_addr(this->_ip_address.c_str());


    startServer();
    std::cout << "TcpServer constructor called" << std::endl;
}

TcpServer::~TcpServer() {
    closeServer();
    std::cout << "TcpDestructor called" << std::endl;
}

int TcpServer::startServer() {
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket < 0)
    {
        std::cerr << "Cannot create socket" < std::endl;
        return 1;
    }
    if (bind(this->_socket,(sockaddr *)&this->_socketAddress, this->_socketAddress_len) < 0)
    {
        std::cerr << "Cannot connect socket to address" << std::endl;
        return 1;
    }
        return 0;
}

void TcpServer::closeServer() {
    close(this->_socket);
}