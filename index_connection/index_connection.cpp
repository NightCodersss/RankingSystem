#include "index_connection.hpp"
#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

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

		std::cerr << "Read json: " << ubjson::to_ostream(request) << '\n';
		std::cout << "Index: Read json: " << ubjson::to_ostream(request) << '\n';

		if(request["query"].isNull())
			return;

		// TODO: move to config
		int packet_size = 1;

		double normalizing_constant = 1e-4; // Cut off this kludge
//		double normalizing_constant = 1; 

		ubjson::StreamWriter<SocketStream> writer(self->ranking_stream);

		auto index_id = static_cast<std::string>(request["index_id"]);
		std::ifstream in("index_" + index_id + ".dat");

		std::cerr << "Index id: " << index_id << '\n';

		std::cerr << "Query-string: " << static_cast<std::string>(request["query"]) <<"\n";
		std::cerr << "Query-string size: " << static_cast<std::string>(request["query"]).size() <<"\n";

		std::cerr << "Before loop\n";
		bool read_file = false;
		while ( !read_file )
		{
			std::cerr << "In loop. Index is ok.\n";
			ubjson::Value result;
			int amount = 0;
			std::vector<ubjson::Value> docs;

			while ( amount < packet_size )
			{
				std::cerr << "Decided to get more words: got " << amount << " of " << packet_size << "\n";
				std::string word;
				DocID doc_id;
				double correspondence;

				if ( in >> word >> doc_id >> correspondence )
				{				
					std::cerr << "Word " << word << " read\n";

					std::cerr << "DOCID " << doc_id << " read\n";
					std::cerr << "Correspondence " << correspondence << " read\n";
					if (word == static_cast<std::string>(request["query"]))
					{
						std::cerr << "Word " << word << " accepted\n";
//						std::cout << "From index server: \n";
						std::cerr << "Doc id: " << doc_id << '\n';

						ubjson::Value doc;
						doc["docid"] = doc_id;
						doc["correspondence"] = correspondence * normalizing_constant;
						doc["docname"] = std::to_string(doc_id);
						doc["url"] = "google.com/" + std::to_string(doc_id);

						docs.push_back(doc);
						amount += 1;
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
			std::cerr << "Amount: " << result["amount"].asInt() << '\n';

			//answer is formed
			std::cerr << "Writing output to ranking server\n";
			writer.writeValue(result);
			std::cerr << "Wrote output to ranking server\n";
		}
	}).detach();
}
    
IndexConnection::IndexConnection(boost::asio::io_service& io_service) { }
