#include <iostream>

#include "../include/Client.hpp"

Client::Client()
	:
	_fd(-1),
	_nick(""),
	_user(""),
	_host(""),
	_name(""),
	_pass(""),
	_silent(true)
{}

Client::~Client()
{
	if (_silent == false)
		std::cout << "Client " << _nick << " disconnected" << std::endl;
}

Client::Client(const Client &src)
	:
	_fd(src._fd),
	_nick(src._nick),
	_user(src._user),
	_host(src._host),
	_name(src._name),
	_pass(src._pass),
	_silent(false)
{}

Client &Client::operator=(const Client &src)
{
	if (this == &src)
		return *this;
	_fd = src._fd;
	_nick = src._nick;
	_user = src._user;
	_host = src._host;
	_name = src._name;
	_pass = src._pass;
	_silent = false;
	return *this;
}

void	Client::setFd(int fd)
{
	_fd = fd;
}

void	Client::setNick(std::string nick)
{
	_nick = nick;
}

void	Client::setUser(std::string user)
{
	_user = user;
}

void	Client::setHost(std::string host)
{
	_host = host;
}

void	Client::setName(std::string name)
{
	_name = name;
}

void	Client::setPass(std::string pass)
{
	_pass = pass;
}

int	Client::getFd(void) const
{
	return _fd;
}

std::string	Client::getNick(void) const
{
	return _nick;
}

std::string	Client::getUser(void) const
{
	return _user;
}

std::string	Client::getHost(void) const
{
	return _host;
}

std::string	Client::getName(void) const
{
	return _name;
}

std::string	Client::getPass(void) const
{
	return _pass;
}
