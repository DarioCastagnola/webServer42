#pragma once
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

class TcpServer
{
    public:
        TcpServer(std::string ip_address, int port);
        ~TcpServer();
        int     startServer();
        void    closeServer();
    private:
        std::string         _ip_address;
        int                 _socket;
        int                 _new_socket;
        int                 _port;
        long                _incoming_message;
        struct  sockaddr_in _socketAddress;
        unsigned int        _socketAddress_len;
        std::string         _server_message;
        
};