#ifndef INDEXCONNECTION_HPP
#define INDEXCONNECTION_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class IndexConnection : public std::enable_shared_from_this<IndexConnection>
{
public:
    typedef std::shared_ptr<IndexConnection> pointer;

    static pointer create(boost::asio::io_service& io_service);

    tcp::socket& get_socket();

    void start();

private:
    IndexConnection(boost::asio::io_service& io_service);

    void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);

    std::string get_output(std::string input);

    tcp::socket socket;
    std::string message;
};

#endif
