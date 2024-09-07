#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>

#include "Client.hpp"
#include "Channel.hpp"

struct Network
{
	struct	sockaddr_in			sa_in;
	struct	sockaddr_storage	sa_storage;
	socklen_t					sa_len;
	int							fd;
	int							status;
	int							port;
	std::string					pass;
	Network()
		:
		sa_in(),
		sa_storage(),
		sa_len(0),
		fd(-1),
		status(0),
		port(6667),
		pass("")
	{};
};

class Client;
class Channel;

class Server
{
	typedef std::vector<pollfd>::iterator pollfd_it;
	typedef std::vector<Client>::iterator client_it;
	typedef std::vector<Channel>::iterator channel_it;

	private:
		std::vector<Client>		_clients;
		std::vector<Channel>	_channels;
		std::vector<pollfd>		_pollfds;
		struct Network			_network;

		// Network initialization
		void					initNetwork(void);
		void					initializeAddress(void);
		void					createSocket(void);
		void					setNonBlocking(void);
		void					bindSocket(void);
		void					listenSocket(void);
		void					initPoll(void);

		// Polling
		void					handlePollin(void);
		void					handlePollout(void);
		void					acceptNewConnection(void);
		pollfd_it 				handleClientMessage(pollfd_it it);

		// Server shutdown
		static bool				shutdown_signal;
		static void				signalHandler(int signum);
		void					shutdown(void);

		// Message sending
		bool 					sendMsg(int fd, const char *msg);

	public:
		Server();
		~Server();

		// Server configuration
		void					setPort(int port);
		void					setPass(std::string pass);

		// Server loop
		void					run(void);

		// Client management
		void					addClient(void);
};

#endif