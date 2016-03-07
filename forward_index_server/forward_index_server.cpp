#include "forward_index_server.hpp"

#include <fstream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

ForwardIndexServer::ForwardIndexServer(boost::asio::io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	load_forward_index();
	BOOST_LOG_TRIVIAL(info) << "Started forward_index server on port: " << port << '\n';
    start_accept();
}
    
void ForwardIndexServer::start_accept()
{
    auto new_connection = ForwardIndexConnection::create(acceptor.get_io_service(), this);

    acceptor.async_accept(*new_connection->ranking_stream.rdbuf(), // socket of stream
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void ForwardIndexServer::handle_accept(ForwardIndexConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
	{
		BOOST_LOG_TRIVIAL(info) << "New forward_index connection\n";
        new_connection->start();
	}
	else
		BOOST_LOG_TRIVIAL(error) << "!!!!! Error at accepting";

    start_accept();
}

void ForwardIndexServer::load_forward_index()
{
	BOOST_LOG_TRIVIAL(trace) << "Begin forward_index load";
	std::string current_forward_index_path = std::string(std::getenv("RANKING_SYSTEM_HOME")) + "/current_index/";

    BOOST_LOG_TRIVIAL(debug) << "ForwardIndex list path: " << current_forward_index_path;

	std::ifstream forward_index_list(current_forward_index_path + "forward_indexlist");

	forward_index.clear();
	std::string forward_index_name;
	std::stringstream ss;
	while (std::getline(forward_index_list, forward_index_name))
	{
		ss.clear();
		ss << forward_index_name;
		DocID doc_id;
		std::string word;
		ss >> doc_id;
		ss >> word;
		std::map<TextID, TextForwardIndexInfo> current = { };
		std::ifstream in(current_forward_index_path + forward_index_name);
		bool read_file = false;
		while ( !read_file )
		{
			std::string word;
			TextID text_id;
			double correspondence;

			if ( in >> text_id >> correspondence )
			{
				current[text_id] = TextForwardIndexInfo{word, doc_id, correspondence, text_id};
			}
			else
			{
				read_file = true;
			}
		}
		in.close();
		forward_index[doc_id][word] = std::move(current);
	}
	forward_index_list.close();
	BOOST_LOG_TRIVIAL(trace) << "End forward_index load";
}

void ForwardIndexServer::inc_connections()
{
	std::lock_guard<std::mutex> lock(connections_mutex);
	++connections;
	BOOST_LOG_TRIVIAL(info) << "Connections uped to: " << connections;
}

void ForwardIndexServer::dec_connections()
{
	std::lock_guard<std::mutex> lock(connections_mutex);
	--connections;
	BOOST_LOG_TRIVIAL(info) << "Connections downed to: " << connections;
}

