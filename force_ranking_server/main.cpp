#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "force_ranking_server.hpp"

int main(int argc, char** argv)
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
		if(argc == 1)
			throw std::logic_error("Usage: server <config file>");
        boost::asio::io_service io_service;
        ForceRankingServer server(io_service, 18000, argv[1]);
        io_service.run();
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Error is caught in main: " << e.what();
    }

    return 0;
}
