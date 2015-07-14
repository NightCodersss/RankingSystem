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

//TODO check the validity of config
 
void RankingConnection::start()
{
	std::thread([self = shared_from_this()]() {
		try
		{
			ubjson::StreamReader<SocketStream> reader(self->south_stream);
			auto request = reader.getNextValue();

			std::cout << "From ranking server: \n";
			std::cout << "Read json: " << ubjson::to_ostream(request) << '\n';

			if(request["query"].isNull())
				return;
			self->index_results.clear();

			std::map<long long, ubjson::Value> docs; //docid, doc
			std::mutex docs_mutex;

			for(const auto& text: self->config["texts"])
			{
				self->index_results.push_back(std::async(std::launch::async, [request, text, &docs, &docs_mutex](){
					try
					{	
						int server_index = 0;
						SocketStream index_stream(text["servers"][server_index]["host"].get<std::string>()
												, text["servers"][server_index]["port"].get<std::string>());
						//Make query for index server
						ubjson::Value query;
						query["query"] = request["query"];
						query["fields"] = {"docname", "url", "docid"};
						query["index_id"] = text["index_id"].get<std::string>();
						
						//Send query
						ubjson::StreamWriter<SocketStream> writer(index_stream);
						writer.writeValue(query);

						//Read answer
						ubjson::StreamReader<SocketStream> reader(index_stream);
						auto res = reader.getNextValue();
					
						//Adding necessary information for next processing
			
						res["factor"] = text["factor"].get<double>();

						std::cout << "Res: " << ubjson::to_ostream(res) << '\n';
						
						//Async processing
						{
							std::lock_guard<std::mutex> lock(docs_mutex);
							for(const auto& doc: res["docs"])
							{
								auto docid = static_cast<long long>(doc["docid"]);
								if(docs.find(docid) == docs.end())
								{
									docs[docid] = doc;
									docs[docid]["rank"] = 0.0;
								}
								static_cast<double&>(docs[docid]["rank"]) += static_cast<double>(res["factor"]);
							}
						}
					}
					catch ( std::exception& e )
					{
						std::cerr << "!!!!!! " << e.what() << '\n';
					}
				}));
			}
			for(auto& res: self->index_results)
			{
				res.wait();
			}
			std::vector<ubjson::Value> docs_vector;
			for(const auto& doc: docs)
			{
				docs_vector.push_back(doc.second);
			}
			std::sort(std::begin(docs_vector), std::end(docs_vector), [](auto&& doc1, auto&& doc2 ) { 
				return static_cast<double>(doc1["rank"]) > static_cast<double>(doc2["rank"]); 
			});
			
			std::size_t res_size = 0;
			ubjson::Value answer;

			std::cout << "Docs vector size: " << docs_vector.size() << "\n";

			for(const auto& doc: docs_vector)
			{
				answer["docs"].push_back(doc);
				++res_size;
				if(!request["amount"].isNull() && res_size >= static_cast<int>(request["amount"])) //TODO: add sup limit
					break;
			}
			answer["amount"] = static_cast<long long>(res_size);
			//answer is formed
			ubjson::StreamWriter<SocketStream> writer(self->south_stream);
			writer.writeValue(answer);
		}
		catch( std::exception& e )
		{
			std::cout << "!!!!!! " << e.what() << '\n';
		}
	}).detach();
}

RankingConnection::RankingConnection(boost::asio::io_service& io_service, const config_type& config): config(config) 
{
}
