#include <cerrno>
#include <cstring>
#include <csignal>

#include "../include/Server.hpp"

#define ERR_ACCEPT_CONNECTION(fd) "Error: Failed to accept new connection on fd: " << fd << " (" << strerror(errno) << ")" << std::endl
#define ERR_MAX_CONNECTIONS "Error: Maximum number of connections reached" << std::endl
#define ERR_SEND_MESSAGE(fd, msg) "Error: Failed to send message to client, fd: " << fd << " (" << strerror(errno) << "), message: " << msg
#define ERR_READ_CLIENT(fd) "Error: Failed to read from client, fd: " << fd << " (" << strerror(errno) << ")" << std::endl
#define ERR_CLOSE_CLIENT(fd) "Error: Failed to close client, fd: " << fd << " (" << strerror(errno) << ")" << std::endl
#define INFO_CLIENT_DISCONNECTED(fd) "Info: Client disconnected, fd: " << fd << std::endl
#define INFO_CLIENT_CONNECTED(fd) "Info: New connection accepted on fd: " << fd << std::endl
#define INFO_OPEN_CONNECTIONS "Info: open connections: " << _pollfds.size() - 1 << std::endl
#define INFO_RECEIVED_MESSAGE(fd, msg) "Info: Received message from client, fd: " << fd << ", message: " << msg << std::endl
#define INFO_SENT_MESSAGE(fd, msg) "Info: Sent message to client, fd: " << fd << ", message: " << msg
#define SIG_CAUGHT_MSG(signal) "Signal caught: " << signalName(signal) << " (" << signal << ")" << std::endl
#define BUFFER_SIZE 512

Server::Server()
{
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);
	signal(SIGUSR1, Server::signalHandler);
	signal(SIGPIPE, SIG_IGN);
	try
	{
		initNetwork();
	}
	catch(const std::exception &e)
	{
		std::cerr << "Network initialization failed: ";
		shutdown_signal = true;
		throw;
	}
}

bool	Server::shutdown_signal = false;

Server::~Server()
{
	for (pollfd_it it = _pollfds.begin(); it != _pollfds.end(); it++)
	{
		if (it->fd != _network.fd)
		{
			bool msg_sent = false;
			it->events = POLLOUT;
			while (!msg_sent)
			{
				if (sendMsg(it->fd, "Server shutting down\n"))
					msg_sent = true;
			}
		}
		if (close(it->fd) == 0)
			std::cout << INFO_CLIENT_DISCONNECTED(it->fd);
		else
			std::cerr << ERR_CLOSE_CLIENT(it->fd);
	}
	std::cout << "Shutting down server gracefully" << std::endl;
}

bool	Server::sendMsg(int fd, const char *msg)
{
	size_t len = strlen(msg);
	ssize_t bytes_sent = 0;
	while (static_cast<size_t>(bytes_sent) < len)
	{
		ssize_t ret = send(fd, msg + bytes_sent, len - bytes_sent, 0);
		if (ret < 0)
		{
			if (errno == EPIPE)
			{
				std::cerr << INFO_CLIENT_DISCONNECTED(fd);
				close(fd);
				for (pollfd_it it = _pollfds.begin(); it != _pollfds.end(); ++it)
				{
					if (it->fd == fd)
					{
						_pollfds.erase(it);
						break;
					}
				}
				std::cout << INFO_OPEN_CONNECTIONS;
				return true;
			}
			std::cerr << ERR_SEND_MESSAGE(fd, msg);
			return false;
		}
		bytes_sent += ret;
	}
	std::cout << INFO_SENT_MESSAGE(fd, msg);
	return true;
}

void	Server::acceptNewConnection(void)
{
    int new_fd = accept(_network.fd, (struct sockaddr *)&_network.sa_storage, &_network.sa_len);

    if (new_fd < 0)
    {
		std::cerr << ERR_ACCEPT_CONNECTION(_network.fd) << std::endl;
        return;
    }

	if (_pollfds.size() > 10)
	{
		std::cerr << ERR_MAX_CONNECTIONS;
		bool msg_sent = false;
		while (!msg_sent)
		{
			if (sendMsg(new_fd, "Server is full\n"))
				msg_sent = true;
		}
		std::cout << INFO_CLIENT_DISCONNECTED(new_fd);
		close(new_fd);
		return;
	}

    pollfd pfd = { new_fd, POLLIN, 0 };
    _pollfds.push_back(pfd);
	std::cout << INFO_CLIENT_CONNECTED(new_fd);
	std::cout << INFO_OPEN_CONNECTIONS;
}

void	Server::handlePollin(void)
{
	for (pollfd_it it = _pollfds.begin(); it != _pollfds.end();)
	{
		if (it->revents & POLLIN)
		{
			if (it->fd == _network.fd)
			{
				acceptNewConnection();
				it = _pollfds.begin() + 1;
			}
			else
				it = handleClientMessage(it);
		}
		else
			++it;
	}
}

Server::pollfd_it	Server::handleClientMessage(pollfd_it pfd)
{
	char buffer[BUFFER_SIZE + 1];
	int	bytes_read = recv(pfd->fd, buffer, BUFFER_SIZE, 0);
	if (bytes_read <= 0)
	{
		if (bytes_read == 0)
		{
			std::cerr << INFO_CLIENT_DISCONNECTED(pfd->fd);
		}
		else
			std::cerr << ERR_READ_CLIENT(pfd->fd) << std::endl;
		close(pfd->fd);
		pfd = _pollfds.erase(pfd);
		std::cout << INFO_OPEN_CONNECTIONS;
	}
	else
	{
		buffer[bytes_read] = '\0';
		std::cout << INFO_RECEIVED_MESSAGE(pfd->fd, buffer);
		pfd->events = POLLOUT;
		++pfd;
	}
	return pfd;
}

void	Server::handlePollout(void)
{
	for (pollfd_it it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->revents & POLLOUT)
		{
			if (sendMsg(it->fd, "pong\n"))
				it->events = POLLIN;
		}
	}
}

void	Server::run(void)
{
	while (!shutdown_signal)
	{
		int	ret = poll(&_pollfds[0], _pollfds.size(), -1);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "Poll error" << std::endl;
			break;
		}
		handlePollin();
		handlePollout();
	}
}

void	Server::setPort(int port)
{
	_network.port = port;
}

void	Server::setPass(std::string pass)
{
	_network.pass = pass;
}

void	Server::addClient(void)
{
	Client	client;
	_clients.push_back(client);
}



void Server::signalHandler(int signal)
{
	switch (signal)
	{
		case SIGINT:
		case SIGQUIT:
		case SIGUSR1:
			std::cerr << SIG_CAUGHT_MSG(signal);
			shutdown_signal = true;
			break;
		default:
			std::cerr << "Unhandled signal caught: " << signal << std::endl;
			break;
	}
}
