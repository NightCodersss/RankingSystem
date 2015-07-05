#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include "index_server.hpp"

int main()
{
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
