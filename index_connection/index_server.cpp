#include "index_server.hpp"

#include <fstream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

IndexServer::IndexServer(boost::asio::io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	load_index();
	BOOST_LOG_TRIVIAL(info) << "Started index server on port: " << port << '\n';
    start_accept();
}
    
void IndexServer::start_accept()
{
    auto new_connection = IndexConnection::create(acceptor.get_io_service(), this);

    acceptor.async_accept(*new_connection->ranking_stream.rdbuf(), // socket of stream
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void IndexServer::handle_accept(IndexConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
	{
		BOOST_LOG_TRIVIAL(info) << "New index connection\n";
        new_connection->start();
	}
	else
		BOOST_LOG_TRIVIAL(error) << "!!!!! Error at accepting";

    start_accept();
}

void IndexServer::load_index()
{
	BOOST_LOG_TRIVIAL(trace) << "Begin index load";
	std::string current_index_path = std::string(std::getenv("RANKING_SYSTEM_HOME")) + "/current_index/";

    BOOST_LOG_TRIVIAL(debug) << "Index list path: " << current_index_path;

	std::ifstream index_list(current_index_path + "indexlist");

	index.clear();
	std::string index_name;
	while (std::getline(index_list, index_name))
	{
		std::vector<TextIndexInfo> current = { };
		std::ifstream in(current_index_path + index_name);
		bool read_file = false;
		while ( !read_file )
		{
			std::string word;
			long long doc_id;
			double correspondence;

			if ( in >> word >> doc_id >> correspondence )
			{
				current.push_back(TextIndexInfo{word, doc_id, correspondence});
			}
			else
			{
				read_file = true;
			}
		}
		in.close();
		index.push_back(std::move(current));
	}
	index_list.close();
	BOOST_LOG_TRIVIAL(trace) << "End index load";
}

void IndexServer::inc_connections()
{
	std::lock_guard<std::mutex> lock(connections_mutex);
	++connections;
	BOOST_LOG_TRIVIAL(info) << "Connections uped to: " << connections;
}

void IndexServer::dec_connections()
{
	std::lock_guard<std::mutex> lock(connections_mutex);
	--connections;
	BOOST_LOG_TRIVIAL(info) << "Connections downed to: " << connections;
}

