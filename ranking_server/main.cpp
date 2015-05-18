#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include "ranking_server.hpp"

int main()
{
    try
    {
        boost::asio::io_service io_service;
        RankingServer server(io_service);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
