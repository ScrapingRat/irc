#include "../include/NetworkInitializer.hpp"
#include "../include/ShutdownManager.hpp"

class NetworkException : public std::runtime_error
{
	public:
		explicit NetworkException(const std::string &message)
		:
		std::runtime_error(message) {}
};

NetworkInitializer::NetworkInitializer()
	:
	port(6667),
	pass("")
{}

NetworkInitializer::NetworkInitializer(int port, const std::string &pass)
	: 
	port(port),
	pass(pass)
{}

NetworkInitializer::~NetworkInitializer() {}

void NetworkInitializer::initialize(void)
{
	try
	{
		initializeAddress();
		createSocket();
		setNonBlocking();
		bindSocket();
		listenSocket();
		initPoll();
		status = 0;
	}
	catch (const NetworkException &e)
	{
		ShutdownManager::getInstance().setShutdownSignal(true);
		throw;
	}
}

void NetworkInitializer::initializeAddress(void)
{
	memset(&sa_in, 0, sizeof(sa_in));
	sa_in.sin_family = AF_INET;
	sa_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sa_in.sin_port = htons(port);
	std::cout << "Server initialized at " << inet_ntoa(sa_in.sin_addr) << ":" << port << std::endl;
}

void NetworkInitializer::createSocket(void)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		throw NetworkException("Failed to create socket");
	}
}

void NetworkInitializer::setNonBlocking(void)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(fd);
		throw NetworkException("Failed to set socket to non-blocking");
	}
}

void NetworkInitializer::bindSocket(void)
{
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(fd);
		throw NetworkException("Failed to set socket options");
	}

	status = bind(fd, (struct sockaddr *)&sa_in, sizeof(sa_in));
	if (status < 0)
	{
		close(fd);
		throw NetworkException("Failed to bind socket");
	}
}

void NetworkInitializer::listenSocket(void)
{
	status = listen(fd, 20);
	if (status < 0)
	{
		close(fd);
		throw NetworkException("Failed to listen on socket");
	}
	sa_len = sizeof(sa_storage);
}

void NetworkInitializer::initPoll(void)
{
	pollfd pfd = { fd, POLLIN, 0 };
	pollfds.push_back(pfd);
}
