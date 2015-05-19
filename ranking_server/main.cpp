#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include "ranking_server.hpp"

int main(int argc, char** argv)
{
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
