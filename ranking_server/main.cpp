#include <ctime>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "ranking_server.hpp"

#include "test.hpp"

int main(int argc, char** argv)
{
	boost::log::add_common_attributes();
	boost::log::add_file_log(
		boost::log::keywords::file_name = "./log.log",
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::format = "[%TimeStamp%] <%ThreadID%> : %Message%"
		);
	boost::log::core::get() -> set_logging_enabled(std::string(std::getenv("RANKING_SERVER_LOGGING")) == "NO" ? false : true);
	BOOST_LOG_TRIVIAL(trace) << "Log begin";

	runTests();

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
        BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
    }

    return 0;
}
