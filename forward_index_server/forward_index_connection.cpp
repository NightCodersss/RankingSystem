#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

#include "forward_index_server.hpp"
#include "forward_index_connection.hpp"

using boost::asio::ip::tcp;
using DocID = long long;

ForwardIndexConnection::pointer ForwardIndexConnection::create(boost::asio::io_service& io_service, ForwardIndexServer* const server)
{
    return pointer(new ForwardIndexConnection(io_service, server));
} 
    
void ForwardIndexConnection::start()
{
	server->inc_connections();
	std::thread([self = shared_from_this()]() {
		ubjson::StreamReader<SocketStream> reader(self->ranking_stream);
		auto request = reader.getNextValue();

		// BOOST_LOG_TRIVIAL(trace) << "Read json: " << ubjson::to_ostream(request) << '\n';
		BOOST_LOG_TRIVIAL(trace) << "Read json.";
	//	std::cout << "ForwardIndex: Read json: " << ubjson::to_ostream(request) << '\n';

		if(request["query"].isNull())
			return;

		// TODO: move to config
		int packet_size = 1;

		double normalizing_constant = 1e-4; // Cut off this kludge
//		double normalizing_constant = 1; 

		ubjson::StreamWriter<SocketStream> writer(self->ranking_stream);
		
		
//		auto forward_index_id = static_cast<std::string>(request["forward_index_id"]);
		int forward_index_id = request["forward_index_id"].asInt();

		BOOST_LOG_TRIVIAL(trace) << "ForwardIndex id: " << forward_index_id << '\n';

		BOOST_LOG_TRIVIAL(trace) << "Query-string: " << static_cast<std::string>(request["query"]) <<"\n";
		BOOST_LOG_TRIVIAL(trace) << "Query-string size: " << static_cast<std::string>(request["query"]).size() <<"\n";

		BOOST_LOG_TRIVIAL(trace) << "Before loop\n";
		int sample_n = 0;
		bool read_file = false;
		while ( !read_file )
		{
			BOOST_LOG_TRIVIAL(trace) << "In loop. ForwardIndex is ok.\n";
			ubjson::Value result;
			int amount = 0;
			std::vector<ubjson::Value> docs;

			while ( amount < packet_size )
			{
				BOOST_LOG_TRIVIAL(trace) << "Decided to get more words: got " << amount << " of " << packet_size << "\n";
				std::string word;
				DocID doc_id;
				double correspondence;

                BOOST_LOG_TRIVIAL(debug) << "Number of servers: " << self->server->forward_index.size();
                BOOST_LOG_TRIVIAL(debug) << "ForwardIndex id: " << forward_index_id;

				BOOST_LOG_TRIVIAL(trace) << "Using sample " << sample_n << " of " << self->server->forward_index[forward_index_id].size();
				if ( sample_n < self->server->forward_index[forward_index_id].size() )
				{	
					const auto& row = self->server->forward_index.at(forward_index_id).at(sample_n);

					word = row.word;
					doc_id = row.doc_id;
					correspondence = row.correspondence;
					++sample_n;

					BOOST_LOG_TRIVIAL(trace) << "Word " << word << " read\n";

					BOOST_LOG_TRIVIAL(trace) << "DOCID " << doc_id << " read\n";
					BOOST_LOG_TRIVIAL(trace) << "Correspondence " << correspondence << " read\n";
					if (word == static_cast<std::string>(request["query"]))
					{
						BOOST_LOG_TRIVIAL(trace) << "Word " << word << " accepted\n";
						BOOST_LOG_TRIVIAL(trace) << "Doc id: " << doc_id << '\n';

						ubjson::Value doc;
						doc["docid"] = doc_id;
						doc["correspondence"] = correspondence * normalizing_constant;
						doc["docname"] = std::to_string(doc_id);
						doc["url"] = "google.com/" + std::to_string(doc_id);

						docs.push_back(doc);
						amount += 1;
					}
				}
				else
				{
					read_file = true;
					break;
				}
			}


			for ( const auto& d : docs )
				result["docs"].push_back(d);

			result["amount"] = amount;
			BOOST_LOG_TRIVIAL(trace) << "Amount: " << result["amount"].asInt() << '\n';

			//answer is formed
			BOOST_LOG_TRIVIAL(trace) << "Writing output to ranking server\n";
			writer.writeValue(result);
			BOOST_LOG_TRIVIAL(trace) << "Wrote output to ranking server\n";
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
    
ForwardIndexConnection::ForwardIndexConnection(boost::asio::io_service& io_service, ForwardIndexServer* const server) : server(server) { }

ForwardIndexConnection::~ForwardIndexConnection()
{
	BOOST_LOG_TRIVIAL(trace) << "Destructor of connection in.";
	server->dec_connections();
}
