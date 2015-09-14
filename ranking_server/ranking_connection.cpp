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

			//TODO memory leak
			auto& docs = *new std::map<DocID, ubjson::Value>(); //docid, doc
			auto& docs_top = *new SortByRankGetByIdWithTop<DocID, double>(10000, 0); // TODO set top_const, bottom_const
			auto& docs_mutex = *new std::mutex;
			auto& mdr_mutex = *new std::mutex;
			bool& is_end = *new bool(false); //NOTE: maybe use std::atomic_flag

			int& download_counter = *new int(0);
	
			//Sum of 𝛎's
			auto& Mdr = *new double(std::accumulate(std::begin(self->config["texts"]), std::end(self->config["texts"]), 0.0, 
			[] (double acc, json text)
			{
				return acc += text["factor"].get<double>();
			})); 
			auto& c = *new std::map<TextID, double>();

			for ( const auto& text: self->config["texts"] )
			{
				auto text_id = text["index_id"].get<TextID>();
				c[text_id] = 1.;
			}

			for(const auto& text: self->config["texts"])
			{
				self->index_results.push_back(std::async(std::launch::async, [&, request, text](){
					try
					{	
						int server_index = 0;
						SocketStream index_stream(text["servers"][server_index]["host"].get<std::string>()
												, text["servers"][server_index]["port"].get<std::string>());
							
						TextID text_id = text["index_id"].get<TextID>();

						//Make query for index server
						ubjson::Value query;
						query["query"] = request["query"];
						query["fields"] = {"docname", "url", "docid"};
						query["index_id"] = text_id;
							
						ubjson::StreamWriter<SocketStream> writer(index_stream);
						ubjson::StreamReader<SocketStream> reader(index_stream);
							
						//Send query
						writer.writeValue(query);

						while ( true ) // TODO wrap fetching into a stream
						{
							//Read answer
							auto res = reader.getNextValue();
							std::cerr << "Came another doc from index server: " << ubjson::to_ostream(res) << '\n';
						
							if ( res["amount"].asInt() == 0 ) // TODO need to understand why static_cast<int> doesn't work
							{
								std::cerr << "Amount is zero. Changing c to 0.\n"; 
								Mdr -= c[text_id] * text["factor"].get<double>();
								c[text_id] = 0;
								Mdr += c[text_id] * text["factor"].get<double>();
								std::cerr << "Thread is going to finish\n";
								break;
							}

							// Adding necessary information for next processing
							res["factor"] = text["factor"].get<double>();

							//Async processing
							// TODO Lock-free
							{
								std::cerr << "Entering lock\n";
								std::lock_guard<std::mutex> lock(docs_mutex);
								for(const auto& doc: res["docs"])
								{									
									auto docid = static_cast<const DocID&>(doc["docid"]);
									download_counter += 1;
									{
//										std::lock_guard<std::mutex> lock(mdr_mutex);
										std::cerr << "Mdr updaing\n";
										Mdr -= c[text_id] * text["factor"].get<double>();
										std::cerr << "Cold: " << c[text_id] << "\n";
										c[text_id] = std::min(c[text_id], static_cast<double>(doc["correspondence"]));
										std::cerr << "Cnew: " << c[text_id] << "\n";
										Mdr += c[text_id] * text["factor"].get<double>();
									}

									if (is_end)
									{
										std::cerr << "Thread is going to finish\n";
										break;
									}

									for ( const auto& doc: docs )
									{
							//			std::cerr << "DocID: " << doc.first << '\n';
//										std::cerr << ubjson::to_ostream(doc.second) << '\n';
									}

									if(docs.find(docid) == docs.end())
									{
										docs[docid] = doc;
									}
									docs_top.increment(docid, static_cast<double>(res["factor"]) * static_cast<double>(doc["correspondence"]));
//									std::cerr << "Top size: " << docs_top.topSize() << '\n';
								}
								std::cerr << "Leaving lock\n";
							}
							if (is_end)
							{
								std::cerr << "Thread is going to finish\n";
								break;
							}
						}
					}
					catch ( std::exception& e )
					{
						std::cerr << "!!!!!! " << e.what() << '\n';
					}
				}));
			}

//			for ( auto& fut : self->index_results )
//			{
//				fut.wait();
//			}


			double C3 = 1.;

			do 
			{
				//std::cerr << "OLOLO TROLOLO\n";
				std::this_thread::yield();
				double tmpMdr;
				{
//					std::lock_guard<std::mutex> lock(mdr_mutex);
					std::cerr << "Entering mdr lock\n";
					std::lock_guard<std::mutex> lock(docs_mutex);
					std::cerr << "Leaving mdr lock\n";
					tmpMdr = Mdr;
				}
	
				//TODO add check of amount
				//TODO change top_const if docs_top.topSize() >= n
	
				double max_diff = 0;
				if ( docs_top.topSize() != 0 )
				{
					std::lock_guard<std::mutex> lock(docs_mutex);
					if ( docs_top.topSize() >= request["amount"].asInt() )
					{
						auto last_in_top = docs_top.topEnd();
						--last_in_top;
						auto new_top_const = last_in_top -> first;
						docs_top.setTopConst(new_top_const);
						docs_top.setBottomConst(new_top_const - tmpMdr);
						// TODO check cutoff
					}

					auto end = docs_top.topEnd();
					--end;
					auto lastAndOne = docs_top.upper_bound(end -> first);

					std::cerr << "Distance: " << std::distance(docs_top.begin(), lastAndOne) << '\n';
					std::cerr << "All size: " << docs_top.size() << '\n';
					for ( auto it = docs_top.begin(); it != lastAndOne; )
					{
						double cur = it -> first;					
						++it;
						double next = it -> first;

						if ( max_diff < cur - next )
							max_diff = cur - next;
					}
				}

				std::cerr << "Max_diff: " << max_diff << "\nMdr: " << tmpMdr << "\n";
				if ( max_diff >= C3 * tmpMdr ) // won't swap
				{
					std::cerr << "Set is_end = true\n";
					is_end = true;
				}

			} while ( !is_end );
			
			std::size_t res_size = 0;
			ubjson::Value answer;

			std::cerr << "Forming answer\n";
			for(const auto& doc: docs_top)
			{
//				std::cerr << "Doc: " << ubjson::to_ostream(docs[doc.second]) << '\n'; 
//				std::cerr << "Doc rank: " << doc.first << '\n';
				docs[doc.second]["rank"] = doc.first;
				answer["docs"].push_back(docs[doc.second]);
				++res_size;
				if( !request["amount"].isNull() && res_size >= request["amount"].asInt() ) //TODO: add sup limit
					break;
			}
			std::cerr << "Answer of ranking server: " << ubjson::to_ostream(answer) << '\n';
			answer["amount"] = static_cast<long long>(res_size);
			//answer is formed

			std::cerr << "Sending formed answer: \n";
			std::cerr << "Downloaded documents: " << download_counter << '\n';

			ubjson::StreamWriter<SocketStream> writer(self->south_stream);
			writer.writeValue(answer);
			
			std::cerr << "Sent formed answer: \n";
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
