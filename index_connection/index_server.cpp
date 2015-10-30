#include "index_server.hpp"

#include <fstream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

const int COUNT_INDEX = 3;
    
IndexServer::IndexServer(boost::asio::io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	load_index();
	BOOST_LOG_TRIVIAL(info) << "Started index server on port: " << port << '\n';
    start_accept();
}
    
void IndexServer::start_accept()
{
    auto new_connection = IndexConnection::create(acceptor.get_io_service());

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
		new_connection->index = &index;
        new_connection->start();
	}
	else
		BOOST_LOG_TRIVIAL(error) << "!!!!! Error at accepting";

    start_accept();
}

void IndexServer::load_index()
{
	BOOST_LOG_TRIVIAL(trace) << "Begin index load";
	std::string current_index_path = "/home/asio/rankingsystem/current_index/";		

	index.clear();
	index.resize(COUNT_INDEX);
	for(int index_id = 0; index_id < COUNT_INDEX; ++index_id)
	{
		std::ifstream in(current_index_path + "index." + std::to_string(index_id) + ".index");
		bool read_file = false;
		while ( !read_file )
		{
			std::string word;
			long long doc_id;
			double correspondence;

			if ( in >> word >> doc_id >> correspondence )
			{
				index[index_id].push_back(TextIndexInfo{word, doc_id, correspondence});
			}
			else
			{
				read_file = true;
			}
		}
		in.close();
	}
	BOOST_LOG_TRIVIAL(trace) << "End index load";
}
