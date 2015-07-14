#include "index_connection.hpp"
#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

using boost::asio::ip::tcp;

ubjson::Value IndexConnection::do_search(ubjson::Value input)
{
	ubjson::Value result;
	int amount = 0;

	std::vector<ubjson::Value> docs;

	if ( static_cast<std::string>(input["query"]) == "Fairytale" )
	{
		auto index_id = static_cast<std::string>(input["index_id"]);

		std::cout << "From index server: \n";
		std::cout << "Index id: " << index_id << '\n';

		std::ifstream in("index_" + index_id + ".dat");
		long long doc_id;
		while ( in >> doc_id )
		{
			std::cout << "From index server: \n";
			std::cout << "Doc id: " << doc_id << '\n';
		
			ubjson::Value doc;
			doc["docid"] = doc_id;
			doc["docname"] = std::to_string(doc_id);
			doc["url"] = "google.com";

			docs.push_back(doc);
			amount += 1;
		}
	}

	for ( const auto& d : docs )
		result["docs"].push_back(d);

	result["amount"] = amount;
	return result;
}

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

		auto answer = self -> do_search(request);

		//answer is formed
		ubjson::StreamWriter<SocketStream> writer(self->ranking_stream);
		writer.writeValue(answer);

	}).detach();
}
    
IndexConnection::IndexConnection(boost::asio::io_service& io_service) { }
