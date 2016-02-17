#include <sstream>
#include <thread>
#include <stream_reader.hpp>
#include <stream_writer.hpp>
#include <batch_sender.hpp>
#include <realtime_sender.hpp>
#include <bitset>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/timer/timer.hpp>

#include "ranking_connection.hpp"
#include "ranking_server.hpp"

using boost::asio::ip::tcp;

RankingConnection::pointer RankingConnection::create(boost::asio::io_service& io_service, const config_type& config, RankingServer* const server)
{
	BOOST_LOG_TRIVIAL(trace) << "Connection creating\n";
    return pointer(new RankingConnection(io_service, config, server));
} 

//TODO check the validity of config

void RankingConnection::start()
{
	server->inc_connections();
	std::thread([self = shared_from_this()]() {
		BOOST_LOG_TRIVIAL(trace) << "Thread of connection has been started\n";
		try
		{
			// TODO: delegate top analics to special class from connection
			ubjson::StreamReader<SocketStream> reader(self->south_stream);
			auto request = reader.getNextValue();

			if(request["query"].isNull())
				return;
			self->index_results.clear();

			auto sender = std::make_unique<BatchSender>(self->south_stream, 10, 1); // TODO: implement policy of root/non-root ranking 
	
			int text_index = 0;
			for(const auto& text: self->config["texts"])
			{
				self->index_results.push_back(std::async(std::launch::async, [&, request, text, text_index](){
					try
					{	 
						boost::timer::cpu_timer t;
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
							
						//Send query to index server
						writer.writeValue(query);

						while ( true ) // TODO wrap fetching into a stream
						{
							boost::timer::cpu_timer network_timer;

							//Read answer
							BOOST_LOG_TRIVIAL(trace) << "Waiting for another document\n";
							auto res = reader.getNextValue();
							BOOST_LOG_TRIVIAL(trace) << "Came another doc from index server " << text_id << text_index << ": ";
						//	BOOST_LOG_TRIVIAL(trace) << ubjson::to_ostream(res) << '\n';

							self->server->log_timer("Came answer in ", network_timer);
						
							if ( res["amount"].asInt() == 0 ) // TODO need to understand why static_cast<int> doesn't work
							{
								BOOST_LOG_TRIVIAL(info) << "Amount is zero. Changing c to 0.\n"; 
								self->data.update_C(text_id, text["factor"].get<double>(), 0);
								BOOST_LOG_TRIVIAL(info) << "Thread is going to finish\n";
								break;
							}

							// Adding necessary information for next processing
							double text_factor = text["factor"].get<double>();

							//Async processing
							// TODO Lock-free
							{
								BOOST_LOG_TRIVIAL(trace) << "Entering lock\n";
								std::lock_guard<std::mutex> lock(self->data.docs_mutex);
								for(const auto& doc: res["docs"])
								{
									DocID docid = static_cast<const DocID&>(doc["docid"]);
									double correspondence = static_cast<double>(doc["correspondence"]);
									self->data.download_counter += 1;
									{
										BOOST_LOG_TRIVIAL(trace) << "Mdr updating\n";
										self->data.update_C(text_id, text_factor, std::min(self->data.c[text_id], correspondence));
									}

									if (self->data.is_end)
									{
										BOOST_LOG_TRIVIAL(info) << "Thread is going to finish\n";
										break;
									}

									self->data.insertText(docid, text_index, Doc(doc), text_factor * correspondence);
								}
								BOOST_LOG_TRIVIAL(trace) << "Leaving lock\n";
							}
							if (self->data.is_end)
							{
								BOOST_LOG_TRIVIAL(info) << "Thread is going to finish\n";
								break;
							}
						}
						self->server->log_timer("Main try", t);
					}
					catch ( std::exception& e )
					{
						BOOST_LOG_TRIVIAL(error) << "!!!!!! " << e.what() << '\n';
					}
				}));
				text_index++;
			}

			double C3 = 1.;
			double max_swap_prob = 0.01;
            int check_size = 100;

			do 
			{
				std::this_thread::yield();

				double Mdr_copy;

				{
					std::lock_guard<std::mutex> lock(self->data.docs_mutex);
					Mdr_copy = self->data.Mdr;
				}

				BOOST_LOG_TRIVIAL(info) << "Mdr: " << Mdr_copy << '\n';
	
				//TODO add check of amount
				//TODO change top_const if docs_top.topSize() >= n
	
				BOOST_LOG_TRIVIAL(info) << "docs_top: " << self->data.docs_top.top_size() << "\n";

                // only for root ranking server: self->data.updateCuttingConsts(request["amount"].asInt(), Mdr_copy);
				if ( self->data.isTheTopDocGoodEnough(self->config, max_swap_prob, check_size) )
				{
                    BOOST_LOG_TRIVIAL(trace) << "The top document is done\n";
                    ubjson::Value answer = self->data.formAnswer();
                    self->data.deleteTheTopDocument();
                    sender->send(answer);

					const auto& err = self->south_stream.error();  
					if ( err )
					{
						BOOST_LOG_TRIVIAL(info) << "Error while writing (to south stream): " << err.message();
						BOOST_LOG_TRIVIAL(trace) << "Error while writing (to south stream), so thread is going down.";
						break;
					}
				}

                //BOOST_LOG_TRIVIAL(info) << "Swap probability: " << swap_prob << '\n';

				const double eps = 1e-10;
                /* TODO: Check for stop in root ranking server and for ranking threshold in all
				if ( swap_prob < max_swap_prob && (self->data.docs_top.top_size() >= request["amount"].asInt() || std::abs(Mdr_copy) < eps)) // won't swap we have got all documents we want and we can
				{
					BOOST_LOG_TRIVIAL(info) << "Set is_end = true\n";
					self->data.is_end = true;
				}
                */

			} while ( !self->data.is_end );
			
			BOOST_LOG_TRIVIAL(info) << "Downloaded documents: " << self->data.download_counter << '\n';
			
			BOOST_LOG_TRIVIAL(trace) << "Shutdowning connection\n";
		}
		catch( std::exception& e )
		{
			std::cout << "!!!!!! " << e.what() << '\n';
			BOOST_LOG_TRIVIAL(error) << "!!!!!! " << e.what();
			BOOST_LOG_TRIVIAL(trace) << "Shutdowning connection\n";
		}
	}).detach();
}

RankingConnection::RankingConnection(boost::asio::io_service& io_service, const config_type& config, RankingServer* server): config(config), data(config), server(server)
{
}

RankingConnection::~RankingConnection()
{
	server->dec_connections();
}
