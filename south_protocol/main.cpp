#include <ctime>
#include <iostream>

#include <boost/asio.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "south_server.hpp"

int main()
{
	boost::log::add_common_attributes();
	boost::log::add_file_log(
		boost::log::keywords::file_name = "./log.log",
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::format = "[%TimeStamp%] <%ThreadID%> : %Message%"
		);
	BOOST_LOG_TRIVIAL(trace) << "Log begin";

    try
    {
        boost::asio::io_service io_service;
        SouthServer server(io_service);
        io_service.run();
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
    }

    return 0;
}
