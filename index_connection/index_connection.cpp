#include "index_connection.hpp"

using boost::asio::ip::tcp;

std::string IndexConnection::get_output(std::string input)
{
    return "Privetiki\n";
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
    message_ = get_output("");

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
            [_this = shared_from_this()](const boost::system::error_code& ec, size_t bytes_transferred) {                   
            _this->handle_write(ec, bytes_transferred);
            });
}
    
IndexConnection::IndexConnection(boost::asio::io_service& io_service) : socket_(io_service) { }
void IndexConnection::handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/) { }
