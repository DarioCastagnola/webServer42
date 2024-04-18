#include "Error_page.hpp"

std::string join(std::vector<std::string> lines, std::string delims)
{
	std::string joined = "";
	
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		joined += lines.at(i);
		if ( i < lines.size()-1)
			joined += delims;
	}
	
	return joined;
}

void Error_page::send_error_page(Server server, std::string uri, int client_fd, int error_code, std::string text)
{
	std::vector<Location> locations = server.get_locations();
	std::string server_name = "Server: "+server.get_server_name();
	std::vector<std::string> response;
	std::stringstream ss;
	
	ss << error_code;
	std::string error_code_str = "HTTP/1.1 "+ss.str()+" Not Found";
	response.push_back(error_code_str);
	response.push_back(server_name);
	response.push_back("Connection: close"); 
	response.push_back("\r\n");
	std::string response_head = Utilities::join(response, "\r\n");

	send(client_fd, response_head.c_str(), response_head.size(), 0);

	for(unsigned int i = 0; i < locations.size(); i++)
	{
		if(locations.at(i).get_path() == uri)
		{
			std::string error_page;
			if ((error_page = search_error_page(locations.at(i).get_error_page(), error_code)) != "")
			{
				std::string error_path = server.get_root() + error_page;
				std::ifstream in_file(error_path.c_str());
	
				if (in_file.is_open()) 
				{

					std::string buffer;

					while(!in_file.eof()) {
						getline(in_file, buffer);
						send(client_fd, buffer.c_str(), buffer.size(), 0);
					}

					in_file.close();
					return;
				}
			}
		}
	}

	std::string error_page;
	if ((error_page = search_error_page(server.get_error_page(), error_code)) != "")
	{
		std::string error_path = server.get_root() + error_page;
		std::ifstream in_file(error_path.c_str());
	
		if (in_file.is_open()) 
		{

			std::string buffer;

			while(!in_file.eof()) {
				getline(in_file, buffer);
				send(client_fd, buffer.c_str(), buffer.size(), 0);
			}

			in_file.close();
			return;
		}
	}
	std::string default_error = "<html><body><center><h2>HTTP/1.1 "+ss.str()+" "+text+"</h2></center></body></html>";
	send(client_fd, default_error.c_str(), default_error.size(), 0);
	return;
}

std::string Error_page::search_error_page(std::vector<error_pages> error_page, int error_code)
{
	for (unsigned int i = 0; i < error_page.size(); i++)
	{
		if (error_page.at(i).code == error_code)
		{
			return error_page.at(i).path;
		}
	}
	return "";
}