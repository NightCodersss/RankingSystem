#include "ranking_connection.hpp"
#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

using boost::asio::ip::tcp;

std::string RankingConnection::getUBJSONFromQuery(std::string input)
{
	using namespace ubjson;
	Value v;
	v["query"] = input;
	
	std::stringstream output;
	StreamWriter<std::stringstream> writer(output);
	writer.writeValue(v);
	return output.str();
}

RankingConnection::pointer RankingConnection::create(boost::asio::io_service& io_service)
{
    return pointer(new RankingConnection(io_service));
} 
 
void RankingConnection::start()
{
	std::thread([self = shared_from_this()]() {
		std::getline(self->client_stream, self->input, '\n');
	
		std::cout << "Read: " << self -> input << '\n';

		self->input = self->getUBJSONFromQuery(self->input);
		self->server_stream << self->input;

		ubjson::StreamReader<SocketStream> reader(self->server_stream);

		auto v = reader.getNextValue();
		self->client_stream << ubjson::to_ostream(v);
	}).detach();
}

RankingConnection::RankingConnection(boost::asio::io_service& io_service) : client(io_service)
{
	server_stream.connect(host, port);
}
