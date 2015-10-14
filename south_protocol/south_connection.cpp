#include "south_connection.hpp"
#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

using boost::asio::ip::tcp;

std::string SouthConnection::getUBJSONFromQuery(std::string input)
{
	using namespace ubjson;
	Value v;
	v["query"] = input;
	v["amount"] = 10;
	
	std::stringstream output;
	StreamWriter<std::stringstream> writer(output);
	writer.writeValue(v);
	return output.str();
}

SouthConnection::pointer SouthConnection::create(boost::asio::io_service& io_service)
{
    return pointer(new SouthConnection(io_service));
} 
 
void SouthConnection::start()
{
	std::thread([self = shared_from_this()]() {
		std::getline(self->client_stream, self->input, '\n');

		if ( self->input.back() == '\n' )
			self->input.pop_back();

		if ( self->input.back() == '\r' )
			self->input.pop_back();
	
		std::cout << "From south server: \n";
		std::cout << "Read: " << self -> input << '\n';
		std::cerr << "From south server: \n";
		std::cerr << "Read: " << self -> input << '\n';

		self->input = self->getUBJSONFromQuery(self->input);
		self->server_stream << self->input;

		ubjson::StreamReader<SocketStream> reader(self->server_stream);

		auto v = reader.getNextValue();
		self->client_stream << "ANSWER: " << ubjson::to_ostream(v);
	}).detach();
}

SouthConnection::SouthConnection(boost::asio::io_service& io_service) : client(io_service)
{
	std::cout << "South server's connection is connecting to ranking\n";
	server_stream.connect(host, port);
}
