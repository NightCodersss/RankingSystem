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

#include <defines.hpp>

using boost::asio::ip::tcp;

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

		BOOST_LOG_TRIVIAL(trace) << "Read json.";
		
		std::stringstream ss;
		ss << ubjson::to_ostream(request);
		BOOST_LOG_TRIVIAL(trace) << "Request: " << ss.str();


		if(request["query"].isNull())
			return;

		ubjson::StreamWriter<SocketStream> writer(self->ranking_stream);
		
		
		DocID doc_id = static_cast<const UbjsonDocID&>(request["doc_id"]);
		std::string query = static_cast<std::string>(request["query"]);

		BOOST_LOG_TRIVIAL(trace) << "Query-string: " << static_cast<std::string>(request["query"]) <<"\n";
		BOOST_LOG_TRIVIAL(trace) << "Query-string size: " << static_cast<std::string>(request["query"]).size() <<"\n";

		ubjson::Value result;

		BOOST_LOG_TRIVIAL(trace) << "Doc id: " << doc_id << "\n";

		std::map<TextID, TextForwardIndexInfo> current = { };
		try {
			std::ifstream in(self->forward_index_path + "/" + std::to_string(doc_id) + ".forward");
			bool read_file = false;
			while ( !read_file )
			{
				TextID text_id;
				double correspondence;
				std::string word; // hash

				if ( in >> word >> text_id >> correspondence )
				{
					if (word == query)
					current[text_id] = TextForwardIndexInfo{word, doc_id, correspondence, text_id};
				}
				else
				{
					read_file = true;
				}
			}
			in.close();
		} catch (std::out_of_range& err) {
			BOOST_LOG_TRIVIAL(info) << "Error: you are asikng word that I dont know: " << err.what();
			return;
		} catch (std::exception& err) {
			BOOST_LOG_TRIVIAL(info) << "Error: " << err.what();
		}

		for (auto const& index_info: current)
		{
			ubjson::Value index_info_json;
			index_info_json["doc_id"] = static_cast<UbjsonDocID>(doc_id);
			index_info_json["text_id"] = index_info.second.text_id; 
			index_info_json["rank"] = index_info.second.correspondence; 
			result.push_back(index_info_json);
		}

		//answer is formed
		{
			std::stringstream ss;
			ss << ubjson::to_ostream(result);
			BOOST_LOG_TRIVIAL(trace) << "Result: " << ss.str();
		}
		BOOST_LOG_TRIVIAL(trace) << "Writing output to ranking server\n";
		writer.writeValue(result);
		BOOST_LOG_TRIVIAL(trace) << "Wrote output to ranking server\n";
		const auto& err = self->ranking_stream.error();  
		if ( err )
		{
			BOOST_LOG_TRIVIAL(info) << "Error while writing: " << err.message();
		}
		BOOST_LOG_TRIVIAL(trace) << "Leaving main lambda function. Resourses going to be freed.";
	}).detach();
}
    
ForwardIndexConnection::ForwardIndexConnection(boost::asio::io_service& io_service, ForwardIndexServer* const server) 
	: server(server) 
	, forward_index_path(std::getenv("FORWARD_INDEX_PATH"))
{ }

ForwardIndexConnection::~ForwardIndexConnection()
{
	BOOST_LOG_TRIVIAL(trace) << "Destructor of connection in.";
	server->dec_connections();
}
