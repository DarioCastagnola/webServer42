#pragma once

#include "../Webserver.hpp"

class Get_request
{
	public:
		Get_request(){};
		~Get_request(){};

		static void get_response(int client_socket, Server server, std::map<std::string, std::string> request_map);
		static bool get_allowed(Server server, std::map<std::string, std::string> request_map);
		static void get_response_directory(int client_socket, Server server, std::map<std::string, std::string> request_map);
		static std::string search_index(Server server, std::map<std::string, std::string> request_map);
};	