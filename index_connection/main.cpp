#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include "index_server.hpp"

int main()
{
	freopen("log.log", "w", stderr);
	std::cerr << "Log begin\n";
//    try
//    {
        boost::asio::io_service io_service;
        IndexServer server(io_service, 16000);
        io_service.run();
//    }
//    catch (std::exception& e)
//    {
//        std::cerr << e.what() << std::endl;
//    }

    return 0;
}
