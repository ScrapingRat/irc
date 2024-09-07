#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
	private:
		int			_fd;
		std::string	_nick;
		std::string	_user;
		std::string	_host;
		std::string	_name;
		std::string	_pass;
		bool		_silent;

	public:
		Client();
		~Client();
		Client(const Client &src);
		Client &operator=(const Client &src);

		void		setFd(int fd);
		void		setNick(std::string nick);
		void		setUser(std::string user);
		void		setHost(std::string host);
		void		setName(std::string name);
		void		setPass(std::string pass);

		int			getFd(void) const;
		std::string	getNick(void) const;
		std::string	getUser(void) const;
		std::string	getHost(void) const;
		std::string	getName(void) const;
		std::string	getPass(void) const;
};

#endif