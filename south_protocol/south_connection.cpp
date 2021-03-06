#include "south_connection.hpp"
#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include "south_server.hpp"

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

SouthConnection::pointer SouthConnection::create(boost::asio::io_service& io_service, SouthServer* const server)
{
    return pointer(new SouthConnection(io_service, server));
} 
 
void SouthConnection::start()
{
	server->inc_connections();
	std::thread([self = shared_from_this()]() {
		std::getline(self->client_stream, self->input, '\n');

		if ( self->input.back() == '\n' )
			self->input.pop_back();

		if ( self->input.back() == '\r' )
			self->input.pop_back();
	
		//std::cout << "From south server: \n";
		//std::cout << "Read: " << self -> input << '\n';
		BOOST_LOG_TRIVIAL(trace) << "From south server:";
		BOOST_LOG_TRIVIAL(trace) << "Read: " << self -> input;

		self->input = self->getUBJSONFromQuery(self->input);
		self->server_stream << self->input;

		ubjson::StreamReader<SocketStream> reader(self->server_stream);

		auto v = reader.getNextValue();
		std::stringstream ss;
		ss << ubjson::to_ostream(v);
		BOOST_LOG_TRIVIAL(trace) << "From ranking server:" << ss.str();
		self->client_stream << "ANSWER: " << ubjson::to_ostream(v) << "\n";
	}).detach();
}

SouthConnection::SouthConnection(boost::asio::io_service& io_service, SouthServer* const server) : client(io_service), server(server)
{
	//std::cout << "South server's connection is connecting to ranking\n";
	BOOST_LOG_TRIVIAL(trace) << "South server's connection is connecting to ranking\n";
	server_stream.connect(host, port);
}

SouthConnection::~SouthConnection()
{
	server->dec_connections();
}
