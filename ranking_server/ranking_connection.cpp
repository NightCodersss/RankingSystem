#include "ranking_connection.hpp"
#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

using boost::asio::ip::tcp;

std::string RankingConnection::getUBJSONFromQuery(std::string input)
{
	using namespace ubjson;
	Value v;
	v["query"] = input;
	
	std::stringstream output;
	StreamWriter<std::stringstream> writer(output);
	writer.writeValue(v);
	return output.str();
}

RankingConnection::pointer RankingConnection::create(boost::asio::io_service& io_service)
{
    return pointer(new RankingConnection(io_service));
} 
 
void RankingConnection::start()
{
	std::thread([self = shared_from_this()]() {
		ubjson::StreamReader<SocketStream> reader(self->south_stream);
		auto v = reader.getNextValue();
		if(v["query"] == ubjson::Value())
			return;
		index_results.clear();
		auto index_server_config = ConfigLoader("index_server").get();
		for(const auto& text: index_server_config["texts"])
		{
			index_results.push_back(std::async([](){
				int server_index = 0;
				SocketStream index_stream(text["servers"][server_index]["host"], text["servers"][server_index]["port"]);
				//Make query for index server
				ubjson::Value query;
				query["query"] = v["query"];
				query["fields"] = {"docname", "url"};
				query["index_id"] = text["index_id"];
				//Send query
				ubjson::StreamWriter<SocketStream> writer(index_stream);
				writer.writeValue(query);
				//Read answer
				ubjson::StreamReader<SocketStream> reader(index_stream);
				auto res = reader.getNextValue();
				//Adding necessary information for next processing
				res["factor"] = text["factor"];
				return res;
			}));
		}
	}).detach();
}

RankingConnection::RankingConnection(boost::asio::io_service& io_service) : client(io_service)
{
	server_stream.connect(host, port);
}
