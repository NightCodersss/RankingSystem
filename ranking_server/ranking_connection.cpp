#include "ranking_connection.hpp"
#include <sstream>
#include <thread>
#include "../config_loader.hpp"
using boost::asio::ip::tcp;

RankingConnection::pointer RankingConnection::create(boost::asio::io_service& io_service)
{
    return pointer(new RankingConnection(io_service));
} 
 
void RankingConnection::start()
{
	std::thread([self = shared_from_this()]() {
		ubjson::StreamReader<SocketStream> reader(self->south_stream);
		auto request = reader.getNextValue();
		if(request["query"] == ubjson::Value())
			return;
		self->index_results.clear();
		auto index_server_config = ConfigLoader("index_server").get();
		for(const auto& text: index_server_config["texts"])
		{
			self->index_results.push_back(std::async([request, text](){
				int server_index = 0;
				SocketStream index_stream(text["servers"][server_index]["host"], text["servers"][server_index]["port"]);
				//Make query for index server
				ubjson::Value query;
				query["query"] = request["query"];
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

RankingConnection::RankingConnection(boost::asio::io_service& io_service) 
{
}
