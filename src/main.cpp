#include "../include/Server.hpp"
#include "../include/utils.hpp"

int main(int ac, char **av)
{
	try
	{
		validateArguments(ac, av);

		int port = parsePort(av[1]);
		const char *password = av[2];

		Server server(port, password);

		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
