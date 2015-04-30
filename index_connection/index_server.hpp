#ifndef INDEXSERVER_HPP
#define INDEXSERVER_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "index_connection.hpp"

class IndexServer
{
public:

    IndexServer(boost::asio::io_service& io_service, int port = 14000);

private:

    void start_accept();
    void handle_accept(IndexConnection::pointer new_connection, const boost::system::error_code& error);
    
    tcp::acceptor acceptor_;
};

#endif
