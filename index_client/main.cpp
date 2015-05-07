#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

using boost::asio::ip::tcp;
using namespace ubjson;

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: client <host> <port>" << std::endl;
			return 1;
		}

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(argv[1], argv[2]);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		tcp::socket socket(io_service);
		boost::asio::connect(socket, endpoint_iterator);

		std::array<char, 1024> buf;
		boost::system::error_code error;
	
		buf = {'O', 'L', 'O', 'L', 'O'};
	
		socket.write_some(boost::asio::buffer(buf.data(), 5));
		for (;;)
		{
			size_t len = socket.read_some(boost::asio::buffer(buf), error);

			std::cout << "Bytes read: " << len << '\n';
			for ( size_t i = 0; i < len; ++i )
				std::cout << buf[i] << ' ';
			std::cout << '\n';

			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			std::stringstream string_stream;
			string_stream.write(buf.data(), len);

			StreamReader<std::stringstream> reader(string_stream);
			auto val = reader.getNextValue();

//			std::cout.write(buf.data(), len);
			std::cout << to_ostream(val) << '\n';
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
