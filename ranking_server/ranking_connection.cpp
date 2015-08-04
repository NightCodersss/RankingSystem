#include "ranking_connection.hpp"
#include <sstream>
#include <thread>
#include <stream_reader.hpp>
#include <stream_writer.hpp>
#include "sortbyrankgetbyidwithtop.hpp"

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

			std::map<DocID, ubjson::Value>& docs = *new std::map<DocID, ubjson::Value>(); //docid, doc
			SortByRankGetByIdWithTop<DocID, double> docs_top(0.8, 0.2); // TODO set top_const, bottom_const
			std::mutex docs_mutex;

			for(const auto& text: self->config["texts"])
			{
				self->index_results.push_back(std::async(std::launch::async, [request, text, &docs, &docs_mutex, &docs_top](){
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
							
						ubjson::StreamWriter<SocketStream> writer(index_stream);
						ubjson::StreamReader<SocketStream> reader(index_stream);
							
						//Send query
						writer.writeValue(query);

						while ( true ) // TODO wrap fetching into a stream
						{
							//Read answer
							auto res = reader.getNextValue();
						
							//Adding necessary information for next processing
							res["factor"] = text["factor"].get<double>();

							if ( static_cast<int>(res["amount"]) == 0 )
								break;

							//Async processing
							//TODO Lock-free
							{
								std::lock_guard<std::mutex> lock(docs_mutex);
								for(const auto& doc: res["docs"])
								{
									auto docid = static_cast<DocID>(doc["docid"]);
									for ( const auto& doc: docs )
									{
										std::cerr << "DocID: " << doc.first << '\n';
										std::cerr << ubjson::to_ostream(doc.second) << '\n';
									}

									if(docs.find(docid) == docs.end())
									{
										docs[docid] = doc;
//										docs[docid]["rank"] = 0.0;
									}
//									static_cast<double&>(docs[docid]["rank"]) += static_cast<double>(res["factor"]);
									docs_top.increment(docid, static_cast<double>(res["factor"]));
								}
							}
						}
					}
					catch ( std::exception& e )
					{
						std::cerr << "!!!!!! " << e.what() << '\n';
					}
				}));
			}

			double C3 = 1.0;

			do 
			{
				std::this_thread::yield();
	
				//TODO add check of amount
				//TODO change top_const if docs_top.topSize() >= n
	
				if ( docs_top.topSize() == 0 )
					continue;

				if ( docs_top.topSize() >= request["amount"] )
				{
					auto last_in_top = docs_top.topEnd();
					--last_in_top;
					auto new_top_const = last_in_top -> second;
					docs_top.setTopConst(new_top_const);
					docs_top.setBottomConst(new_top_const - Mdr);
				}
				double max_diff = 0;

				auto end = docs_top.topEnd();
				--end;
				auto lastAndOne = docs_top.upper_bound(*end);

				for ( auto it = docs_top.begin(); it != lastAndOne; )
				{
					double cur = it -> second;
					++it;
					double next = it -> second;
					
					if ( max_diff < cur - next )
						max_diff = cur - next;
				}

				if ( max_diff > C3 * Mdr ) // won't swap
					break;

			} while ( true );
			
			std::size_t res_size = 0;
			ubjson::Value answer;

			for(const auto& doc: docs_top)
			{
				answer["docs"].push_back(docs[doc.first]);
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
