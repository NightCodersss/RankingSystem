#include "ranking_connection.hpp"
#include <sstream>
#include <thread>
#include <stream_reader.hpp>
#include <stream_writer.hpp>
#include <bitset>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

using boost::asio::ip::tcp;

RankingConnection::pointer RankingConnection::create(boost::asio::io_service& io_service, const config_type& config)
{
	BOOST_LOG_TRIVIAL(trace) << "Connection creating\n";
    return pointer(new RankingConnection(io_service, config));
} 

//TODO check the validity of config

void RankingConnection::start()
{
	std::thread([self = shared_from_this()]() {
		BOOST_LOG_TRIVIAL(trace) << "Thread of connection has been started\n";
		try
		{
			ubjson::StreamReader<SocketStream> reader(self->south_stream);
			auto request = reader.getNextValue();

//			std::cout << "From ranking server: \n";
//			std::cout << "Read json: " << ubjson::to_ostream(request) << '\n';

			if(request["query"].isNull())
				return;
			self->index_results.clear();

	
			int text_index = 0;
			for(const auto& text: self->config["texts"])
			{
				self->index_results.push_back(std::async(std::launch::async, [&, request, text, text_index](){
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
							BOOST_LOG_TRIVIAL(trace) << "Waiting for another document\n";
							auto res = reader.getNextValue();
							BOOST_LOG_TRIVIAL(trace) << "Came another doc from index server " << text_id << text_index << ": ";
						//	BOOST_LOG_TRIVIAL(trace) << ubjson::to_ostream(res) << '\n';
						
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
					}
					catch ( std::exception& e )
					{
						BOOST_LOG_TRIVIAL(error) << "!!!!!! " << e.what() << '\n';
					}
				}));
				text_index++;
			}

			double C3 = 1.;
			double max_swap_prob = 0.7;

			do 
			{
				std::this_thread::yield();

				double Mdr_copy;

				{
					std::lock_guard<std::mutex> lock(self->data.docs_mutex);
					Mdr_copy = self->data.Mdr;
				}

				BOOST_LOG_TRIVIAL(trace) << "Mdr " << Mdr_copy << '\n';
	
				//TODO add check of amount
				//TODO change top_const if docs_top.topSize() >= n
	
				BOOST_LOG_TRIVIAL(trace) << "docs_top: " << self->data.docs_top.top_size() << "\n";

				double swap_prob = 0;
				if ( self->data.docs_top.top_size() >= 2 )
				{
					self->data.updateRankingConsts(request["amount"].asInt(), Mdr_copy);
					swap_prob = self->data.computeSwapProbability(self->config);
				}

				BOOST_LOG_TRIVIAL(trace) << "Swap probability: " << swap_prob << '\n';

				const double eps = 1e-10;
				if ( swap_prob < max_swap_prob && (self->data.docs_top.top_size() >= request["amount"].asInt() || std::abs(Mdr_copy) < eps)) // won't swap we have got all documents we want and we can
				{
					BOOST_LOG_TRIVIAL(info) << "Set is_end = true\n";
					self->data.is_end = true;
				}

			} while ( !self->data.is_end );
			
			std::size_t res_size = 0;
			ubjson::Value answer;

			BOOST_LOG_TRIVIAL(trace) << "Forming answer\n";
			
			answer = self->data.formAnswer(request["amount"].isNull() ? -1 : request["amount"].asInt());
	//		BOOST_LOG_TRIVIAL(trace) << "Answer of ranking server: " << ubjson::to_ostream(answer) << '\n';
			//answer is formed

			BOOST_LOG_TRIVIAL(trace) << "Sending formed answer: \n";
			BOOST_LOG_TRIVIAL(info) << "Downloaded documents: " << self->data.download_counter << '\n';

			ubjson::StreamWriter<SocketStream> writer(self->south_stream);
			writer.writeValue(answer);
			
			BOOST_LOG_TRIVIAL(trace) << "Sent formed answer: \n";
		}
		catch( std::exception& e )
		{
			std::cout << "!!!!!! " << e.what() << '\n';
			BOOST_LOG_TRIVIAL(error) << "!!!!!! " << e.what();
		}
	}).detach();
}

RankingConnection::RankingConnection(boost::asio::io_service& io_service, const config_type& config): config(config), data(config)
{
}
