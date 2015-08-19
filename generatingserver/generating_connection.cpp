#include "generating_connection.hpp"
#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"
#include <defines.hpp>

using boost::asio::ip::tcp;
using DocID = long long;

GeneratingConnection::pointer GeneratingConnection::create(boost::asio::io_service& io_service)
{
    return pointer(new GeneratingConnection(io_service));
} 
    
void GeneratingConnection::start()
{
	std::thread([self = shared_from_this()]() {
		ubjson::StreamReader<SocketStream> reader(self->ranking_stream);
		auto request = reader.getNextValue();

		if(request["query"].isNull())
			return;

		int packet_size = 100;

		ubjson::StreamWriter<SocketStream> writer(self->ranking_stream);

		std::random_device rd;
		std::mt19937 gen(rd());

		double range = std::accumulate(

		for ( int amount = 0; amount < docs_amount; )
		{
			ubjson::Value result;
			std::vector<ubjson::Value> docs;
		
			for ( int i = 0; i < packet_size && amount < docs_amount; ++i, ++amount )
			{
				std::uniform_real_distribution<> dis(0, range);
				lambda = dis(gen);
				range -= lambda * c;
			}

			for ( const auto& d : docs )
				result["docs"].push_back(d);

			result["amount"] = amount;

			//answer is formed
			writer.writeValue(result);
		}
	}).detach();
}
    
GeneratingConnection::GeneratingConnection(boost::asio::io_service& io_service) { }
