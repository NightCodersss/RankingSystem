#include "index_connection.hpp"
#include <sstream>
//#include "../UbjsonCpp/include/value.hpp"
//#include "../UbjsonCpp/include/stream_reader.hpp"
//#include "../UbjsonCpp/include/stream_writer.hpp"

using boost::asio::ip::tcp;

std::string IndexConnection::get_output(std::string input)
{
/*
	using namespace ubjson;
	Value v;
	v["input"] = input;	
	v["output"] = 1;
	v["ololo"] = {1, 2};
	v["gjfijgfi"]["errwr"] = {1, 2, {3, 4}};
	
	std::stringstream output;
	StreamWriter<std::stringstream> writer(output);
	writer.writeValue(v);
*/
//	return output.str();
	return input + '\n' + "Privetiki\n";
}

IndexConnection::pointer IndexConnection::create(boost::asio::io_service& io_service)
{
    return pointer(new IndexConnection(io_service));
} 
 
tcp::socket& IndexConnection::socket()
{
    return socket_;
}
    
void IndexConnection::start()
{
    message_.resize(100);

    boost::asio::async_read(socket_, boost::asio::mutable_buffers_1(&message_[0], 5),
		[_this = shared_from_this()](const boost::system::error_code& error_code, std::size_t bytes_transferred) {
			std::cout << "Bytes read: " << bytes_transferred << '\n';
			for ( int i = 0; i < bytes_transferred; ++i )
				std::cout << (int)_this->message_[i] << ' ';
			std::cout << '\n';
			_this->message_ = _this->get_output(_this->message_);
			boost::asio::async_write(_this->socket_, boost::asio::buffer(_this->message_),
				[_this = _this](const boost::system::error_code& ec, size_t bytes_transferred) {                   
					_this->handle_write(ec, bytes_transferred);
				});
		});

}
    
IndexConnection::IndexConnection(boost::asio::io_service& io_service) : socket_(io_service) { }
void IndexConnection::handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/) { }
