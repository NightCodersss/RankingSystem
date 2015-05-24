#include "ranking_connection.hpp"
#include <sstream>
#include <thread>
#include <stream_reader.hpp>
#include <stream_writer.hpp>

using boost::asio::ip::tcp;

RankingConnection::pointer RankingConnection::create(boost::asio::io_service& io_service, const config_type& config)
{
    return pointer(new RankingConnection(io_service, config));
} 
 
void RankingConnection::start()
{
	std::thread([self = shared_from_this()]() {
		ubjson::StreamReader<SocketStream> reader(self->south_stream);
		auto request = reader.getNextValue();

		std::cout << "Read json: " << ubjson::to_ostream(request) << '\n';

		if(request["query"].isNull())
			return;
		self->index_results.clear();
		for(const auto& text: self->config["texts"])
		{
			self->index_results.push_back(std::async([request, text](){
				int server_index = 0;
				SocketStream index_stream(text["servers"][server_index]["host"].get<std::string>(), text["servers"][server_index]["port"].get<std::string>());
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

RankingConnection::RankingConnection(boost::asio::io_service& io_service, const config_type& config): config(config) 
{
}
