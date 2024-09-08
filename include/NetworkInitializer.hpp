#ifndef NETWORK_INITIALIZER_HPP
#define NETWORK_INITIALIZER_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <string>
#include <unistd.h>
#include <vector>

class NetworkInitializer
{
	private:
		void						initializeAddress(void);
		void						createSocket(void);
		void						setNonBlocking(void);
		void						bindSocket(void);
		void						listenSocket(void);
		void						initPoll(void);
		void 						closeSocketWithError(const std::string &message);

	public:
		std::vector<pollfd>			pollfds;
		struct	sockaddr_in			sa_in;
		struct	sockaddr_storage	sa_storage;
		socklen_t					sa_len;
		int							fd;
		int							port;
		std::string					pass;

		NetworkInitializer();
		NetworkInitializer(int port, const std::string &pass);
		~NetworkInitializer();

		void						initialize(void);
};

#endif // NETWORK_INITIALIZER_HPP