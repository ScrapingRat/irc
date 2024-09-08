#ifndef UTILS_HPP
#define UTILS_HPP

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

int			parsePort(const char *portStr);
std::string	signalName(int signal);
void		closeClient(int fd);
void		validateArguments(int ac, char **av);

#endif // UTILS_HPP