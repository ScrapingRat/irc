#include <iostream>
#include <unistd.h>

#include <../include/utils.hpp>

#define ERR_CLOSE_CLIENT(fd) "Error: Failed to close client, fd: " << fd << " (" << strerror(errno) << ")" << std::endl
#define INFO_CLIENT_DISCONNECTED(fd) "Info: Client disconnected, fd: " << fd << std::endl

void	closeClient(int fd)
{
	if (close(fd) == 0)
		std::cout << INFO_CLIENT_DISCONNECTED(fd);
	else
		std::cerr << ERR_CLOSE_CLIENT(fd);
}
