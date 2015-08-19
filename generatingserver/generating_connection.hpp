#ifndef INDEXCONNECTION_HPP
#define INDEXCONNECTION_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <value.hpp>
#include <defines.hpp>

using boost::asio::ip::tcp;

class GeneratingConnection : public std::enable_shared_from_this<GeneratingConnection>
{
	friend class IndexServer;
public:
    using pointer = std::shared_ptr<GeneratingConnection>;
	using SocketStream = boost::asio::ip::tcp::iostream;

    static pointer create(boost::asio::io_service& io_service);

    void start();

private:
    GeneratingConnection(boost::asio::io_service& io_service);

    SocketStream ranking_stream;
};

#endif
