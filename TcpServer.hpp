#pragma once
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 30720

class TcpServer
{
    public:
        TcpServer(std::string ip_address, int port, char *file);
        ~TcpServer();
        int			startServer();
        int			closeServer();
        int			startListen();
        int			acceptConnection(int &newsocket);
		void		sendResponse();
		std::string	buildResponse();
    private:
        char                *_conf_file;
        std::string         _ip_address;
        int                 _socket;
        int                 _new_socket;
        int                 _port;
        long                _incoming_message;
        struct  sockaddr_in _socketAddress;
        unsigned int        _socketAddress_len;
        std::string         _server_message;
        
};