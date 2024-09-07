#include "../include/Channel.hpp"

Channel::Channel()
	:
	_name(""),
	_clients(),
	_operators(),
	_topic(""),
	_creator("")
{}

Channel::~Channel()
{}
