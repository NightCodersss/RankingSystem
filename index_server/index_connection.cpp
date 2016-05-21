#include <sstream>
#include <thread>
#include <cstdlib>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <common/realtime_sender.hpp>

#include <keyvaluestorage.hpp>
#include <rankdocserializer.hpp>
#include <bigstorage.hpp>

#include "index_server.hpp"
#include "index_connection.hpp"

#include <defines.hpp>

using boost::asio::ip::tcp;

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

		double idf = self->server->idf.GetIDF(word);
		BOOST_LOG_TRIVIAL(trace) << "Query-string size: " << word.size() <<"\n";

		BOOST_LOG_TRIVIAL(trace) << "Before loop\n";
		
		try 
		{
			auto big_storage = BigStorage(std::getenv("INDEX_PATH"));
			BOOST_LOG_TRIVIAL(trace) << "Index path: " << big_storage.getFilename(word, index_id);
			auto storage = big_storage.getForwardStorage(word, index_id);
			for (auto it = storage->getIterator(); it.hasNext(); ++it)
			{
				std::uint64_t doc_id = it->second;
				double correspondence = it->first; // At indexing we assume correspandence is tf 

				BOOST_LOG_TRIVIAL(debug) << "Index id: " << index_id;

				BOOST_LOG_TRIVIAL(trace) << "DOCID: " << doc_id << " read";
				BOOST_LOG_TRIVIAL(trace) << "Correspondence: " << correspondence << " read";

				double rank = correspondence * idf;
				BOOST_LOG_TRIVIAL(trace) << "Rank: " << rank;

				ubjson::Value doc;
				doc["doc_id"] = static_cast<UbjsonDocID>(doc_id);
				doc["rank"] = rank;
				doc["docname"] = std::to_string(doc_id);
				doc["url"] = "google.com/" + std::to_string(doc_id);
				doc["amount"] = 1;

				BOOST_LOG_TRIVIAL(trace) << "Before send to sender";
				sender->send(doc);

				const auto& err = self->ranking_stream.error();  
				if ( err )
				{
					BOOST_LOG_TRIVIAL(info) << "Error while writing: " << err.message();
					BOOST_LOG_TRIVIAL(trace) << "Error while writing, so thread is going down.";
					break;
				}
			}
			ubjson::Value doc;
			doc["is_end"] = 1;
			sender->send(doc);
		}
		catch (const std::exception& err)
		{
			BOOST_LOG_TRIVIAL(info) << "Error: " << err.what();
			ubjson::Value doc;
			doc["is_end"] = 1;
			sender->send(doc);
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
