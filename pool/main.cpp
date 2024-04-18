#include "Webserver.hpp"
#include "./parser_conf/Parser.hpp"

int main(int argc, char *argv[], char *envp[]) {
	
	std::string config_file = "webserver.conf";
	std::vector<Server> servers;

	(void)argv;
	(void)envp;

	if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << "<config_file>" << std::endl;
		return -1;
	}
	
	if (argc == 2)
		servers = Parser::config(argv[1]);
	else
		servers = Parser::config(config_file.c_str());

	//ciclo tutti i server e se trovo delle porte uguali esco
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
		for (std::vector<Server>::iterator it2 = it + 1; it2 != servers.end(); ++it2) {
			if (it->get_listen() == it2->get_listen()) {
				std::cerr << "Error: two servers cannot listen on the same port" << std::endl;
				return -1;
			}
		}
	}
	
	Webserver webserver = Webserver();
	webserver.run(servers);
	
	return 0;

}