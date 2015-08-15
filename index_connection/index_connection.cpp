#include "index_connection.hpp"
#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"
#include <defines.hpp>

using boost::asio::ip::tcp;
using DocID = long long;

IndexConnection::pointer IndexConnection::create(boost::asio::io_service& io_service)
{
    return pointer(new IndexConnection(io_service));
} 
    
void IndexConnection::start()
{
	std::thread([self = shared_from_this()]() {
		ubjson::StreamReader<SocketStream> reader(self->ranking_stream);
		auto request = reader.getNextValue();

		std::cout << "From index server: \n";
		std::cout << "Read json: " << ubjson::to_ostream(request) << '\n';

		if(request["query"].isNull())
			return;

		int packet_size = 100;

		ubjson::StreamWriter<SocketStream> writer(self->ranking_stream);

		auto index_id = static_cast<std::string>(request["index_id"]);
		std::ifstream in("index_" + index_id + ".txt");

		std::cout << "From index server: \n";
		std::cout << "Index id: " << index_id << '\n';

		std::vector<ubjson::Value> docs;


		std::cerr << "Pizdoh before loop\n";
		bool read_file = false;
		while ( !read_file )
		{
			std::cerr << "Ne pizdoh in loop\n";
			ubjson::Value result;
			int amount = 0;

			while ( amount < packet_size )
			{
				std::string word;
				DocID doc_id;
				double correspondence;

				std::cerr << "Pizdoh\n";
				if ( in >> word )
				{
					std::cerr << "Word: " << word << '\n';
					if ( in >> doc_id )
					{
						std::cerr << "DocID: " << doc_id << '\n';
						if ( in >> correspondence )
						{
							std::cerr << "Correspondence: " << correspondence << '\n';
							if (word == static_cast<std::string>(request["query"]))
							{
								std::cerr << "Ne pizdoh\n";
								std::cout << "From index server: \n";
								std::cout << "Doc id: " << doc_id << '\n';

								ubjson::Value doc;
				std::cerr << "Pizdoh\n";
								doc["docid"] = doc_id;
				std::cerr << "Ne pizdoh\n";
				std::cerr << "Pizdoh\n";
								doc["correspondence"] = correspondence;
				std::cerr << "Ne pizdoh\n";
				std::cerr << "Pizdoh\n";
								doc["docname"] = std::to_string(doc_id);
				std::cerr << "Ne pizdoh\n";
				std::cerr << "Pizdoh\n";
								doc["url"] = "google.com/" + std::to_string(doc_id);
				std::cerr << "Ne pizdoh\n";

				std::cerr << "Pizdoh\n";
								docs.push_back(doc);
				std::cerr << "Ne pizdoh\n";
								amount += 1;
							}
							else
							{
								read_file = true;
								break;
							}
						}
						else
						{
							read_file = true;
							break;
						}
					}
					else
					{
						read_file = true;
						break;
					}
				}
				else
				{
					read_file = true;
					break;
				}
			}

			for ( const auto& d : docs )
				result["docs"].push_back(d);

			result["amount"] = amount;

			//answer is formed
			writer.writeValue(result);
		}
	}).detach();
}
    
IndexConnection::IndexConnection(boost::asio::io_service& io_service) { }
