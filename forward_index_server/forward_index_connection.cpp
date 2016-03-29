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

		// BOOST_LOG_TRIVIAL(trace) << "Read json: " << ubjson::to_ostream(request) << '\n';
		BOOST_LOG_TRIVIAL(trace) << "Read json.";
	//	std::cout << "ForwardIndex: Read json: " << ubjson::to_ostream(request) << '\n';

		if(request["query"].isNull())
			return;

		ubjson::StreamWriter<SocketStream> writer(self->ranking_stream);
		
		
//		int forward_index_id = request["forward_index_id"].asInt();
		DocID doc_id = request["doc_id"].asInt();
		std::string query = static_cast<std::string>(request["query"]);

		BOOST_LOG_TRIVIAL(trace) << "Query-string: " << static_cast<std::string>(request["query"]) <<"\n";
		BOOST_LOG_TRIVIAL(trace) << "Query-string size: " << static_cast<std::string>(request["query"]).size() <<"\n";

		ubjson::Value result;

		BOOST_LOG_TRIVIAL(trace) << "Doc id: " << doc_id << "\n";
		BOOST_LOG_TRIVIAL(trace) << "Forward index size: " << self->server->forward_index.size() << "\n";

		const auto& indexes_info = self->server->forward_index.at(doc_id).at(query);
		for (auto const& index_info: indexes_info)
		{
			ubjson::Value index_info_json;
			index_info_json["doc_id"] = doc_id;
			index_info_json["text_id"] = index_info.second.text_id; 
			index_info_json["rank"] = index_info.second.correspondence; 
			result.push_back(index_info_json);
		}
		//answer is formed
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
    
ForwardIndexConnection::ForwardIndexConnection(boost::asio::io_service& io_service, ForwardIndexServer* const server) : server(server) { }

ForwardIndexConnection::~ForwardIndexConnection()
{
	BOOST_LOG_TRIVIAL(trace) << "Destructor of connection in.";
	server->dec_connections();
}
