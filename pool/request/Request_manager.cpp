#include "Request_manager.hpp"

std::map<std::string, std::string> Request_manager::parse_request(std::string file){
	
	std::map<std::string, std::string> request_map;
	std::string line;
	std::istringstream file_stream(file);

	std::vector<std::string> request_and_params = Utilities::split(file, "\n", 1);
	std::vector<std::string> request = Utilities::split(request_and_params.at(0), " ", 1);
	if (request.size() != 3)
	{
		request_map["error"] = "400";
		return request_map;
	}
	request_map["request"] = request.at(0);
	request_map["path"] = request.at(1);
	request_map["http_version"] = request.at(2);

	while (std::getline(file_stream, line))
	{
		std::string key;
		std::string value;
		std::istringstream line_stream(line);
		std::getline(line_stream, key, ':');
		std::getline(line_stream, value);
		Utilities::trim(key);
		Utilities::trim(value);
		request_map[key] = value;
	}

	std::string hostname = request_map["Host"];
	std::vector<std::string> host = Utilities::split(hostname, ":", 1);
	if (host.size() == 2)
	{
		request_map["host"] = Utilities::trim(host.at(0));
		request_map["port"] = Utilities::trim(host.at(1));
	}
	else
	{
		request_map["host"] = Utilities::trim(host.at(0));
		request_map["port"] = "80";
	}

	if (request_map["path"] == "")
		request_map["path"] = "/";
	
	if (request_map["path"][request_map["path"].size()-1 ] == '/' && request_map["path"].size() > 1)
		{
			std::string path(request_map["path"].begin(), request_map["path"].end()-1);
			request_map["path"] = path;
		}

	return request_map;
}

std::vector<char> Request_manager::recive_request(int client_socket)
{
	std::vector<char> message;
	char buffer[1];
	int bytes_read = 0;
	
	while((bytes_read = recv(client_socket, buffer, 1, 0)) > 0)
	{
		message.push_back(buffer[0]);
		if (message.size() > 4 && message.at(message.size()-4) == '\r' && message.at(message.size()-3) == '\n' && message.at(message.size()-2) == '\r' && message.at(message.size()-1) == '\n')
			break;
	}
	
	return message;
}

std::vector<char> Request_manager::get_request_body(std::vector<char> message)
{
	std::vector<char> message_body;

	for (size_t i = 0; i < message.size(); i++)
		message_body.push_back(message.at(i));
	
	while (message_body.size() > 4 && message_body.at(0) != '\r' && message_body.at(1) != '\n' && message_body.at(2) != '\r' && message_body.at(3) != '\n')
		message_body.erase(message_body.begin());
	
	return message_body;
}

std::map<std::string, std::string> Request_manager::set_file(Server server, std::map<std::string, std::string> request_map)
{
	std::string uri = server.get_root() + request_map["path"];

	struct stat st;

	request_map["file"] = "";

	if(stat(uri.c_str(), &st) == 0)
	{
		if((st.st_mode & S_IFMT) == S_IFDIR)
		{
			if (uri[uri.size()-1] != '/')
				request_map["path"] += "/";
		}
		else
		{
			std::vector<std::string> file = Utilities::split(uri, "/", 1);
			request_map["file"] = file.at(file.size()-1);
			std::string path(request_map["path"].begin(), request_map["path"].end()-request_map["file"].size());
			request_map["path"] = path;
		}
	}
	else {
		std::vector<std::string> file = Utilities::split(uri, "/", 1);
		request_map["file"] = file.at(file.size()-1);
		std::string path(request_map["path"].begin(), request_map["path"].end()-request_map["file"].size());
		request_map["path"] = path;
		}

	return request_map;
}