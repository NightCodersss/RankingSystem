#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include "ranking_server.hpp"

#include "test.hpp"

int main(int argc, char** argv)
{
	runTests();

	freopen("log.log", "w", stderr);
	std::cerr << "Log begin\n";

    try
    {
		if(argc == 1)
			throw std::logic_error("Usage: server <config file>");
        boost::asio::io_service io_service;
        RankingServer server(io_service, argv[1]);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
