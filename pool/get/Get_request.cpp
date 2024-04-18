#include "Get_request.hpp"

bool Get_request::get_allowed(Server server, std::map<std::string, std::string> request_map)
{
	std::vector<Location> locations = server.get_locations();
	for(unsigned int i = 0; i < locations.size(); i++)
	{
		if(locations.at(i).get_path() == request_map["path"])
		{
			if (locations.at(i).get_allow().GET == false && server.get_allow().POST == false && server.get_allow().DELETE == false)
				return true; // IMPOSTO GET DI DEFAULT A TRUE
			else
				return locations.at(i).get_allow().GET;
		}
	}
	if (server.get_allow().GET == false && server.get_allow().POST == false && server.get_allow().DELETE == false)
		return true; // IMPOSTO GET DI DEFAULT A TRUE
	return server.get_allow().GET;
}

void Get_request::get_response(int client_sock, Server server, std::map<std::string, std::string> request_map)
{

	if (Get_request::get_allowed(server, request_map) == false) {
		Error_page::send_error_page(server, request_map["path"], client_sock, 405, "Method Not Allowed");
		return;
	}

	std::string uri = server.get_root() + request_map["path"];

	if (request_map["path"]+request_map["file"] == "/favicon.ico") {
		std::string response = "HTTP/1.1 200 OK\r\n\r\n";
		send(client_sock, response.c_str(), response.size(), 0);
		close(client_sock);
		return;
	}

	request_map = Request_manager::set_file(server, request_map);
	
	if (request_map["file"] == "") {
		Get_request::get_response_directory(client_sock, server, request_map);
		return;
	}

	std::vector<std::string> response;
	std::string server_name = "Server: "+server.get_server_name();
	response.push_back("HTTP/1.1 200 OK");
	response.push_back(server_name);
	response.push_back("Connection: close");
	response.push_back("\r\n");
	
	std::string response_head = Utilities::join(response, "\r\n");
	
	std::ifstream in_file(uri.c_str());
	
	if (in_file.is_open()) {
		send(client_sock, response_head.c_str(), response_head.size(), 0);
		std::string buffer;
		while(!in_file.eof()) {
			getline(in_file, buffer);
			send(client_sock, buffer.c_str(), buffer.size(), 0);
		}
		in_file.close();
		return;
	}

	Error_page::send_error_page(server, request_map["path"], client_sock, 404, "Not Found");
}

void Get_request::get_response_directory(int client_sock, Server server, std::map<std::string, std::string> request_map)
{
	std::string uri = server.get_root() + request_map["path"];
	std::vector<std::string> response;
	std::string server_name = "Server: "+server.get_server_name();
	response.push_back("HTTP/1.1 200 OK");
	response.push_back(server_name);
	response.push_back("Connection: close");
	response.push_back("\r\n");
	
	std::string response_head = Utilities::join(response, "\r\n");
	
	request_map["file"] = Get_request::search_index(server, request_map);

	uri = server.get_root() + request_map["path"] +"/"+ request_map["file"];
	
	struct stat st;

	if(stat(uri.c_str(), &st) == 0)
	{
		send(client_sock, response_head.c_str(), response_head.size(), 0);
		std::ifstream in_file(uri.c_str());
		if (in_file.is_open()) 
		{
			std::string buffer;
			std::string response = ""; 
			while(!in_file.eof()) {
				getline(in_file, buffer);
				response += buffer;
			}
			send(client_sock, response.c_str(), response.size(), 0);
			in_file.close();
		}
		return;
	}

	std::vector<Location> locations = server.get_locations();

	for(unsigned int i = 0; i < locations.size(); i++)
	{
		if(locations.at(i).get_path() == request_map["path"])
		{
			if (locations.at(i).get_autoindex() == true)
			{
				send(client_sock, response_head.c_str(), response_head.size(), 0);
				std::string response_body = "<html><head><title>Index of "+request_map["path"]+"</title></head><body><h1>Index of "+request_map["path"]+"</h1><hr><pre>";
				send(client_sock, response_body.c_str(), response_body.size(), 0);
				return;
			}
			else
			{
				Error_page::send_error_page(server, request_map["path"], client_sock, 404, "Not Found");
				return;
			}
		}
	}

	if(server.get_autoindex() == true)
	{
	    // send(client_sock, response_head.c_str(), response_head.size(), 0);
		// std::string response_body = "<html><head><title>Index of "+request_map["path"]+"</title></head><body><h1>Index of "+request_map["path"]+"</h1><hr><pre>";
		// send(client_sock, response_body.c_str(), response_body.size(), 0);
		AutoIndex::createAutoIndex(server, "/");
		return;
	}
	else 
		Error_page::send_error_page(server, request_map["path"], client_sock, 404, "Not Found");
}

std::string Get_request::search_index(Server server, std::map<std::string, std::string> request_map)
{
	std::vector<Location> locations = server.get_locations();
	for(unsigned int i = 0; i < locations.size(); i++)
	{
		if(locations.at(i).get_path() == request_map["path"])
			return locations.at(i).get_index();
	}
	return server.get_index();
}

