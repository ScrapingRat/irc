#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
	private:
		std::string				_name;
		std::vector<Client*>	_clients;
		std::vector<Client*>	_operators;
		std::string				_topic;
		std::string				_creator;

	public:
		Channel();
		~Channel();
};

#endif