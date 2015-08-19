#ifndef GENERATINGSERVER_HPP
#define GENERATINGSERVER_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "generating_connection.hpp"

class GeneratingServer
{
public:

    GeneratingServer(boost::asio::io_service& io_service, int port);

private:

    void start_accept();
    void handle_accept(GeneratingConnection::pointer new_connection, const boost::system::error_code& error);
    
    tcp::acceptor acceptor;
};

#endif
