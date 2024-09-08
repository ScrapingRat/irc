#include "../include/utils.hpp"

#define ERR_CLOSE_CLIENT(fd) "Error: Failed to close client, fd: " << fd << " (" << strerror(errno) << ")" << std::endl
#define INFO_CLIENT_DISCONNECTED(fd) "Info: Client disconnected, fd: " << fd << std::endl

void	closeClient(int fd)
{
	if (close(fd) == 0)
		std::cout << INFO_CLIENT_DISCONNECTED(fd);
	else
		std::cerr << ERR_CLOSE_CLIENT(fd);
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
