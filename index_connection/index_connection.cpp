#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <common/realtime_sender.hpp>

#include "index_server.hpp"
#include "index_connection.hpp"

using boost::asio::ip::tcp;
using DocID = long long;

IndexConnection::pointer IndexConnection::create(boost::asio::io_service& io_service, IndexServer* const server)
{
    return pointer(new IndexConnection(io_service, server));
} 
    
void IndexConnection::start()
{
	server->inc_connections();
	std::thread([self = shared_from_this()]() {
		ubjson::StreamReader<SocketStream> reader(self->ranking_stream);
		auto request = reader.getNextValue();
			
		auto sender = std::make_unique<RealTimeSender>(self->ranking_stream); 

		BOOST_LOG_TRIVIAL(trace) << "Read json.";

		if(request["query"].isNull())
			return;

		TextID index_id = static_cast<TextID>(request["index_id"]);
		BOOST_LOG_TRIVIAL(trace) << "Index id: " << index_id << '\n';

		std::string word = static_cast<std::string>(request["query"]);

		BOOST_LOG_TRIVIAL(trace) << "Query-string: " << word <<"\n";
		BOOST_LOG_TRIVIAL(trace) << "Query-string size: " << word.size() <<"\n";

		BOOST_LOG_TRIVIAL(trace) << "Before loop\n";
		
		const auto& word_text_vector = self->server->index.at(word).at(index_id);

		for (const auto& index_info: word_text_vector)
		{
			BOOST_LOG_TRIVIAL(trace) << "In loop. Index is ok.\n";

			DocID doc_id = index_info.doc_id;
			double correspondence = index_info.correspondence;

			BOOST_LOG_TRIVIAL(debug) << "Number of servers: " << self->server->index.size();
			BOOST_LOG_TRIVIAL(debug) << "Index id: " << index_id;

			BOOST_LOG_TRIVIAL(trace) << "DOCID " << doc_id << " read\n";
			BOOST_LOG_TRIVIAL(trace) << "Correspondence " << correspondence << " read\n";

			ubjson::Value doc;
			doc["docid"] = doc_id;
			doc["correspondence"] = correspondence;
			doc["docname"] = std::to_string(doc_id);
			doc["url"] = "google.com/" + std::to_string(doc_id);

			sender->send(doc);

			const auto& err = self->ranking_stream.error();  
			if ( err )
			{
				BOOST_LOG_TRIVIAL(info) << "Error while writing: " << err.message();
				BOOST_LOG_TRIVIAL(trace) << "Error while writing, so thread is going down.";
				break;
			}
		}

		BOOST_LOG_TRIVIAL(trace) << "Leaving main lambda function. Resourses going to be freed.";
	}).detach();
}
    
IndexConnection::IndexConnection(boost::asio::io_service& io_service, IndexServer* const server) : server(server) { }

IndexConnection::~IndexConnection()
{
	BOOST_LOG_TRIVIAL(trace) << "Destructor of connection in.";
	server->dec_connections();
}
