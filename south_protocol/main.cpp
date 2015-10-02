#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include "south_server.hpp"

int main()
{
	freopen("log.log", "w", stderr);
	std::cerr << "Log begin\n";

    try
    {
        boost::asio::io_service io_service;
        SouthServer server(io_service);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
