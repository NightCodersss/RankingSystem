#ifndef INDEXCONNECTION_HPP
#define INDEXCONNECTION_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <value.hpp>

#include "text_index_info.hpp"

using boost::asio::ip::tcp;

class IndexConnection : public std::enable_shared_from_this<IndexConnection>
{
	friend class IndexServer;
public:
    using pointer = std::shared_ptr<IndexConnection>;
	using SocketStream = boost::asio::ip::tcp::iostream;

    static pointer create(boost::asio::io_service& io_service);

    void start();

private:
    IndexConnection(boost::asio::io_service& io_service);

	std::vector<std::vector<TextIndexInfo>> const * index;

    SocketStream ranking_stream;
};

#endif
