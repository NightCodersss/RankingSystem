#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

#include "forward_index_server.hpp"

int main()
{
	boost::log::add_file_log(
		boost::log::keywords::file_name = "./forward.log",
		boost::log::keywords::auto_flush = true
		);
	BOOST_LOG_TRIVIAL(trace) << "Log begin";

//    try
//    {
        boost::asio::io_service io_service;
        ForwardIndexServer server(io_service, 17000);
        io_service.run();
//    }
//    catch (std::exception& e)
//    {
//        std::cerr << e.what() << std::endl;
//    }

    return 0;
}
