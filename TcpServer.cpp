#include "TcpServer.hpp"

TcpServer::TcpServer(std::string ip_address, int port): _ip_address(ip_address), _port(port), _socketAddress() {
    std::cout << "TcpServer constructor called" << std::endl;
    this->_new_socket = 0;
    this->_socket = 0;
    this->_incoming_message = 0;
    this->_socketAddress_len = sizeof(this->_socketAddress);
    this->_socketAddress.sin_family = AF_INET;
    this->_socketAddress.sin_port = htons(this->_port);
    this->_socketAddress.sin_addr.s_addr = inet_addr(this->_ip_address.c_str());
    this->_server_message = buildResponse();

    if (startServer() == -1) {
		std::ostringstream ss;
		ss << "Failed to start server with PORT: " << ntohs(this->_socketAddress.sin_port);
		std::cout << ss.str() << std::endl;
        closeServer();
	}
}

TcpServer::~TcpServer() {
    std::cout << "TcpServer destructor called" << std::endl;
    closeServer();
}

int TcpServer::startServer() {
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket < 0)
    {
        std::cerr << "Cannot create socket" << std::endl;
        return -1;
    }
    else
    {
      std::cout << "Socket created" << std::endl;
    }
    if (bind(this->_socket,(sockaddr *)&this->_socketAddress, this->_socketAddress_len) < 0)
    {
        std::cerr << "Cannot bind socket to address: " << this->_ip_address << std::endl;
        return -1;
    }
    else
    {
      std::cout << "Socket binded to address: " << this->_ip_address << std::endl;
    }
    return 0;
}

int TcpServer::closeServer() {
    if (close(this->_socket) == 0) {
		std::cout << "Socket binded to: " << this->_ip_address << " has been successfully closed" << std::endl;
		return 0;
	}
	else {
		std::cerr << "Socket closure has failed" << std::endl;
		return -1;
	}
	if (close(this->_new_socket) == 0) {
		std::cout << "New Socket binded to: " << this->_ip_address << " has been successfully closed" << std::endl;
		return 0;
	}
	else {
		std::cerr << "New Socket closure has failed" << std::endl;
		return -1;
	}
}

int TcpServer::startListen() {
    if (listen(this->_socket, 20) < 0){
        std::cerr << "Socket listen failed" << std::endl;
		return -1;
	}
    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: " 
        << inet_ntoa(this->_socketAddress.sin_addr) 
        << " PORT: " << ntohs(this->_socketAddress.sin_port) 
        << " ***\n";
    std::cout << ss.str() << std::endl;
	int bytesReceived;
	while (true)
	{
		std::cout << "====== Waiting for a new connection ======" << std::endl;
		acceptConnection(this->_new_socket);

		char buffer[BUFFER_SIZE] = {0};
		bytesReceived = read(this->_new_socket, buffer, BUFFER_SIZE);
		if (bytesReceived < 0)
			std::cerr << "Failed to read bytes from client socket connection" << std::endl;
		std::ostringstream ss;
		ss << "------ Received Request from client ------\n";
		std::cout << ss.str() << std::endl;

		sendResponse();

		close(this->_new_socket);
	}
	return 0;
}

int TcpServer::acceptConnection(int &new_socket) {
    new_socket = accept(this->_socket, (sockaddr *)&this->_socketAddress, 
                        &this->_socketAddress_len);
    if (this->_new_socket < 0)
    {
        std::ostringstream ss;
        ss << 
        "Server failed to accept incoming connection from ADDRESS: " 
        << inet_ntoa(this->_socketAddress.sin_addr) << "; PORT: " 
        << ntohs(this->_socketAddress.sin_port);
        std::cerr << ss.str() << std::endl;
		return -1;
    }
	return 0;
}

std::string TcpServer::buildResponse() {
	std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
		<< htmlFile;

	return ss.str();
}

void TcpServer::sendResponse() {
	long bytesSent;
    std::cout << "START MESSAGE" << std::endl;
    std::cout << this->_server_message.c_str() << std::endl;
    std::cout << "FINISHED MESSAGE" << std::endl;
	bytesSent = write(this->_new_socket, this->_server_message.c_str(), this->_server_message.size());

	if (bytesSent == this->_server_message.size())
		std::cout << "------ Server Response sent to client ------\n\n" << std::endl;
	else
		std::cout << "Error sending response to client" << std::endl;
}