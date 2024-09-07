#include <cstring>
#include <arpa/inet.h>
#include <fcntl.h>

#include "../include/Server.hpp"

class NetworkException : public std::runtime_error
{
	public:
		explicit NetworkException(const std::string &message) : std::runtime_error(message) {}
};

void Server::initNetwork(void)
{
	try
	{
		initializeAddress();
		createSocket();
		setNonBlocking();
		bindSocket();
		listenSocket();
		initPoll();
		_network.status = 0;
	}
	catch (const NetworkException &e)
	{
		shutdown_signal = true;
		throw;
	}
}

void Server::initializeAddress(void)
{
	memset(&_network.sa_in, 0, sizeof(_network.sa_in));
	_network.sa_in.sin_family = AF_INET;
	_network.sa_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	_network.sa_in.sin_port = htons(_network.port);
	std::cout << "Server initialized at " << inet_ntoa(_network.sa_in.sin_addr) << ":" << _network.port << std::endl;
}

void Server::createSocket(void)
{
	_network.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_network.fd == -1)
	{
		throw NetworkException("Failed to create socket");
	}
}

void Server::setNonBlocking(void)
{
	if (fcntl(_network.fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(_network.fd);
		throw NetworkException("Failed to set socket to non-blocking");
	}
}

void Server::bindSocket(void)
{
	int opt = 1;
	if (setsockopt(_network.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_network.fd);
		throw NetworkException("Failed to set socket options");
	}

	_network.status = bind(_network.fd, (struct sockaddr *)&_network.sa_in, sizeof(_network.sa_in));
	if (_network.status < 0)
	{
		close(_network.fd);
		throw NetworkException("Failed to bind socket");
	}
}

void Server::listenSocket(void)
{
	_network.status = listen(_network.fd, 20);
	if (_network.status < 0)
	{
		close(_network.fd);
		throw NetworkException("Failed to listen on socket");
	}
	_network.sa_len = sizeof(_network.sa_storage);
}

void Server::initPoll(void)
{
	pollfd pfd = { _network.fd, POLLIN, 0 };
	_pollfds.push_back(pfd);
}
