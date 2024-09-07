#include <sstream>

#include "../include/Server.hpp"

void validateArguments(int ac, char **av);
int parsePort(const char *portStr);
void setupAndRunServer(int port, const char *password);

int main(int ac, char **av)
{
	try
	{
		validateArguments(ac, av);
		int port = parsePort(av[1]);
		setupAndRunServer(port, av[2]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void validateArguments(int ac, char **av)
{
	if (ac != 3)
	{
		throw std::invalid_argument("Usage: " + std::string(av[0]) + " <port> <password>");
	}
}

int parsePort(const char *portStr)
{
	std::stringstream ss(portStr);
	int port;
	if (!(ss >> port))
	{
		throw std::invalid_argument("Invalid port");
	}
	return port;
}

void setupAndRunServer(int port, const char *password)
{
	Server server;
	server.setPort(port);
	server.setPass(password);
	server.run();
}

std::string	signalName(int signal)
{
	switch (signal)
	{
		case SIGINT:
			return "SIGINT";
		case SIGQUIT:
			return "SIGQUIT";
		case SIGUSR1:
			return "SIGUSR1";
		default:
			return "UNKNOWN";
	}
}
