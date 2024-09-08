#include <cerrno>
#include <cstring>
#include <csignal>

#include "../include/Server.hpp"
#include "../include/utils.hpp"
#include "../include/ShutdownManager.hpp"

#define ERR_ACCEPT_CONNECTION(fd) "Error: Failed to accept new connection on fd: " << fd << " (" << strerror(errno) << ")" << std::endl
#define ERR_MAX_CONNECTIONS "Error: Maximum number of connections reached" << std::endl
#define ERR_SEND_MESSAGE(fd, msg) "Error: Failed to send message to client, fd: " << fd << " (" << strerror(errno) << "), message: " << msg
#define ERR_READ_CLIENT(fd) "Error: Failed to read from client, fd: " << fd << " (" << strerror(errno) << ")" << std::endl
#define ERR_CLOSE_CLIENT(fd) "Error: Failed to close client, fd: " << fd << " (" << strerror(errno) << ")" << std::endl
#define INFO_CLIENT_DISCONNECTED(fd) "Info: Client disconnected, fd: " << fd << std::endl
#define INFO_CLIENT_CONNECTED(fd) "Info: New connection accepted on fd: " << fd << std::endl
#define INFO_OPEN_CONNECTIONS "Info: open connections: " << _network.pollfds.size() - 1 << std::endl
#define INFO_RECEIVED_MESSAGE(fd, msg) "Info: Received message from client, fd: " << fd << ", message: " << msg << std::endl
#define INFO_SENT_MESSAGE(fd, msg) "Info: Sent message to client, fd: " << fd << ", message: " << msg
#define SIG_CAUGHT_MSG(signal) "Signal caught: " << signalName(signal) << " (" << signal << ")" << std::endl
#define BUFFER_SIZE 512
#define MAX_CONNECTIONS 10
#define PFD _network.pollfds

Server::Server(int port, const std::string &pass)
{
	setupSignalHandlers();
	initializeNetwork(port, pass);
}

Server::~Server()
{
	for (pollfd_it it = PFD.begin(); it != PFD.end(); ++it)
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
		closeClient(it->fd);
		std::cout << INFO_OPEN_CONNECTIONS;
	}
	std::cout << "Shutting down server gracefully" << std::endl;
}

void	Server::setupSignalHandlers(void)
{
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);
	signal(SIGUSR1, Server::signalHandler);
	signal(SIGPIPE, SIG_IGN);
}

void	Server::initializeNetwork(int port, const std::string &pass)
{
	try
	{
		_network.port = port;
		_network.pass = pass;
		_network.initialize();
	}
	catch(const std::exception &e)
	{
		ShutdownManager::getInstance().setShutdownSignal(true);
		throw;
	}
}

void	Server::handleBrokenPipe(int fd)
{
	closeClient(fd);
	for (pollfd_it it = PFD.begin(); it != PFD.end(); ++it)
	{
		if (it->fd == fd)
		{
			PFD.erase(it);
			break;
		}
	}
	std::cout << INFO_OPEN_CONNECTIONS;
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
				handleBrokenPipe(fd);
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

void Server::handleMaxConnections(int fd)
{
    std::cerr << ERR_MAX_CONNECTIONS;
    bool msg_sent = false;
    while (!msg_sent)
    {
        if (sendMsg(fd, "Server is full\n"))
            msg_sent = true;
    }
    closeClient(fd);
}

void	Server::acceptNewConnection(void)
{
    int new_fd = accept(_network.fd, (struct sockaddr *)&_network.sa_storage, &_network.sa_len);

    if (new_fd < 0)
    {
		std::cerr << ERR_ACCEPT_CONNECTION(_network.fd) << std::endl;
    }
	else if (PFD.size() > MAX_CONNECTIONS)
	{
		handleMaxConnections(new_fd);
	}
	else
	{
		addNewConnection(new_fd);
	}
}

void	Server::addNewConnection(int new_fd)
{
	pollfd pfd = { new_fd, POLLIN, 0 };
	PFD.push_back(pfd);
	std::cout << INFO_CLIENT_CONNECTED(new_fd);
	std::cout << INFO_OPEN_CONNECTIONS;
}

void	Server::handlePollin(void)
{
	for (pollfd_it it = PFD.begin(); it != PFD.end();)
	{
		if (it->revents & POLLIN)
		{
			if (it->fd == _network.fd)
			{
				acceptNewConnection();
				it = PFD.begin() + 1;
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
		if (bytes_read < 0)
			std::cerr << ERR_READ_CLIENT(pfd->fd) << std::endl;
		closeClient(pfd->fd);
		pfd = PFD.erase(pfd);
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
	for (pollfd_it it = PFD.begin(); it != PFD.end(); ++it)
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
	while (!ShutdownManager::getInstance().getShutdownSignal())
	{
		int	ret = poll(&PFD[0], PFD.size(), -1);
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
			ShutdownManager::getInstance().setShutdownSignal(true);
			break;
		default:
			std::cerr << "Unhandled signal caught: " << signal << std::endl;
			break;
	}
}
