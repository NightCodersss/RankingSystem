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

	result = input;

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
