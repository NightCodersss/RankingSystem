#include "ranking_connection.hpp"
#include <sstream>
#include <thread>
#include <stream_reader.hpp>
#include <stream_writer.hpp>
#include <bitset>

using boost::asio::ip::tcp;

RankingConnection::pointer RankingConnection::create(boost::asio::io_service& io_service, const config_type& config)
{
	std::cerr << "Connection creating\n";
    return pointer(new RankingConnection(io_service, config));
} 

//TODO check the validity of config

RankingConnection::RankingSystemData::RankingSystemData(config_type const& config)
{
	//Sum of 𝛎's
	Mdr = 0.;

	for ( const auto& text: config["texts"] )
	{
		auto text_id = text["index_id"].get<TextID>();
		c[text_id] = 1.;
		Mdr += text["factor"].get<double>();
	}

}
void RankingConnection::RankingSystemData::update_C(TextID text_id, double factor, double new_val)
{
	Mdr -= c[text_id] * factor;
	c[text_id] = new_val;
	Mdr += c[text_id] * factor;
}
		
void RankingConnection::RankingSystemData::insertText(DocID docid, TextIndex text_index, const Doc& doc, double delta)
{
	if(docs.find(docid) == docs.end())
	{
		docs[docid] = doc;
	}
	if (!docs[docid].got[text_index])
	{
		docs[docid].got[text_index] = 1;
		docs_top.increment(docid, delta);
	}
}

ubjson::Value RankingConnection::RankingSystemData::formAnswer(long long amount)
{
	ubjson::Value answer;

	long long res_size = 0;

	for(const auto& doc: docs_top)
	{
		docs[doc.second].doc["rank"] = doc.first;
		answer["docs"].push_back(docs[doc.second].doc);
		++res_size;
		if (amount != -1 && res_size >= amount)//TODO: add sup limit
 			break;
	}
	answer["amount"] = res_size;
	return answer;
}

void RankingConnection::RankingSystemData::updateRankingConsts(long long amount, double tmpMdr)
{
	std::lock_guard<std::mutex> lock(docs_mutex);
	if (docs_top.topSize() >= amount)
	{
		auto last_in_top = docs_top.end();

		for (int i = 0; i < docs_top.topSize() - amount + 1; ++i)
		{			
			--last_in_top;
		}

		auto new_top_const = last_in_top -> first;
		docs_top.setTopConst(new_top_const);
		docs_top.setBottomConst(new_top_const - tmpMdr);
		docs_top.cutOff();
	}
}

double RankingConnection::RankingSystemData::computeSwapProbability(config_type const& config)
{
	auto end = docs_top.end(); // End of docs_top.top
	--end; //Last of docs_top.top
	auto lastAndOne = docs_top.upper_bound(end -> first); // prev elem to (last in top) in docs_top.all // TODO: change method name to all_upper_bound

	double swap_prob = 0;

	const double eps = 1e-6;

	auto it = docs_top.allBegin();
	for ( ; it != lastAndOne; ++it ) // *it is (rank_of_doc, doc_id)
	{
		docs[it -> second].update(config, c);
	}
	docs[it -> second].update(config, c);

	for (it = docs_top.allBegin(); it != lastAndOne; ) // *it is (rank_of_doc, doc_id)
	{
		double x1 = it -> first;					
		double dx1 = docs[it -> second].mdr;

		++it;

		double x2 = it -> first;
		double dx2 = docs[it -> second].mdr;

		double M = std::min(x1 + dx1, x2 + dx2);
		double m = std::max(x1, x2);

		double this_swap_prob;

		if(std::abs(dx1) > eps && std::abs(dx2) > eps)
			this_swap_prob = (x1 + dx1) * (M - m) / dx1 / dx2 - (M*M - m*m)/(2 * dx1 * dx2);
		else if (std::abs(dx1) < eps)
			this_swap_prob = 0;
		else 
			this_swap_prob = (x1 + dx1 - x2) / dx1;

		if (this_swap_prob < 0)
			this_swap_prob = 0;

		swap_prob += this_swap_prob;
	}

	return swap_prob;
}

RankingConnection::Doc::Doc(const ubjson::Value& d) : doc(d) { }

void RankingConnection::Doc::update(json const& config, auto const& c)
{
	int text_index = 0;
	mdr = 0;
	for ( const auto& text: config["texts"] )
	{
		auto text_id = text["index_id"].get<TextID>();
		mdr += c.at(text_id) * !got[text_index];
	}
}

void RankingConnection::start()
{
	std::thread([self = shared_from_this()]() {
		std::cerr << "Thread of connection has been started\n";
		try
		{
			ubjson::StreamReader<SocketStream> reader(self->south_stream);
			auto request = reader.getNextValue();

			std::cout << "From ranking server: \n";
			std::cout << "Read json: " << ubjson::to_ostream(request) << '\n';

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
							std::cerr << "Waiting for another document\n";
							auto res = reader.getNextValue();
							std::cerr << "Came another doc from index server " << text_id << text_index << ": ";
							std::cerr << ubjson::to_ostream(res) << '\n';
						
							if ( res["amount"].asInt() == 0 ) // TODO need to understand why static_cast<int> doesn't work
							{
								std::cerr << "Amount is zero. Changing c to 0.\n"; 
								self->data.update_C(text_id, text["factor"].get<double>(), 0);
								std::cerr << "Thread is going to finish\n";
								break;
							}

							// Adding necessary information for next processing
							double text_factor = text["factor"].get<double>();

							//Async processing
							// TODO Lock-free
							{
								std::cerr << "Entering lock\n";
								std::lock_guard<std::mutex> lock(self->data.docs_mutex);
								try
								{
									for(const auto& doc: res["docs"])
									{
										DocID docid = static_cast<const DocID&>(doc["docid"]);
										double correspondence = static_cast<double>(doc["correspondence"]);
										self->data.download_counter += 1;
										{
	//										std::lock_guard<std::mutex> lock(mdr_mutex);
											std::cerr << "Mdr updating\n";
											self->data.update_C(text_id, text_factor, std::min(self->data.c[text_id], correspondence));
										}

										if (self->data.is_end)
										{
											std::cerr << "Thread is going to finish\n";
											break;
										}

										self->data.insertText(docid, text_index, Doc(doc), text_factor * correspondence);
									}
								}
								catch (std::exception& e)
								{
									std::cerr << "!!!! Problem in for: " << e.what() << '\n';
								}
								std::cerr << "Leaving lock\n";
							}
							if (self->data.is_end)
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
				text_index++;
			}

			double C3 = 1.;
			double max_swap_prob = 0.7;

			do 
			{
				std::this_thread::yield();
				double tmpMdr;
				{
//					std::lock_guard<std::mutex> lock(mdr_mutex);
//					std::cerr << "Entering mdr lock\n";
					std::lock_guard<std::mutex> lock(self->data.docs_mutex);
//					std::cerr << "Leaving mdr lock\n";
					tmpMdr = self->data.Mdr;
				}

				std::cerr << "Mdr " << tmpMdr << '\n';
	
				//TODO add check of amount
				//TODO change top_const if docs_top.topSize() >= n
	
				double swap_prob = 0;
				std::cerr << "docs_top: " << self->data.docs_top.topSize() << "\n";
				if ( self->data.docs_top.topSize() >= 2 )
				{
					self->data.updateRankingConsts(request["amount"].asInt(), tmpMdr);
					swap_prob = self->data.computeSwapProbability(self->config);
				}

				std::cerr << "Swap probability: " << swap_prob << '\n';

				const double eps = 1e-10;
				if ( swap_prob < max_swap_prob && (self->data.docs_top.topSize() >= request["amount"].asInt() || std::abs(tmpMdr) < eps)) // won't swap we have got all documents we want and we can
				{
					std::cerr << "Set is_end = true\n";
					self->data.is_end = true;
				}

			} while ( !self->data.is_end );
			
			std::size_t res_size = 0;
			ubjson::Value answer;

			std::cerr << "Forming answer\n";
			
			answer = self->data.formAnswer(request["amount"].isNull() ? -1 : request["amount"].asInt());
			std::cerr << "Answer of ranking server: " << ubjson::to_ostream(answer) << '\n';
			//answer is formed

			std::cerr << "Sending formed answer: \n";
			std::cerr << "Downloaded documents: " << self->data.download_counter << '\n';

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

RankingConnection::RankingConnection(boost::asio::io_service& io_service, const config_type& config): config(config), data(config)
{
}
