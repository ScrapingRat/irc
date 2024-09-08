#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>

#include "Client.hpp"
#include "Channel.hpp"
#include "NetworkInitializer.hpp"

std::string	signalName(int signal);

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
		NetworkInitializer		_network;

		// Server initialization
		void					initializeNetwork(int port, const std::string &pass);
		void					setupSignalHandlers(void);

		// Polling
		void					handlePollin(void);
		void					handlePollout(void);
		void					acceptNewConnection(void);
		pollfd_it 				handleClientMessage(pollfd_it it);

		// Server shutdown
		static void				signalHandler(int signum);

		// Message sending
		bool 					sendMsg(int fd, const char *msg);
		void					handleBrokenPipe(int fd);

	public:
		Server(int port, const std::string &pass);
		~Server();

		// Server loop
		void					run(void);

		// Client management
		void					addClient(void);
		void					handleMaxConnections(int fd);
		void					addNewConnection(int fd);
};

#endif