#ifndef FORCE_RANKINGCONNECTION_HPP
#define FORCE_RANKINGCONNECTION_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <value.hpp>
#include <defines.hpp>

using boost::asio::ip::tcp;

class ForceRankingServer;

class ForceRankingConnection : public std::enable_shared_from_this<ForceRankingConnection>
{
	friend class ForceRankingServer;
public:
    using pointer = std::shared_ptr<ForceRankingConnection>;
	using SocketStream = boost::asio::ip::tcp::iostream;

    static pointer create(boost::asio::io_service& io_service, ForceRankingServer * const server);

    void start();

	~ForceRankingConnection();

private:
    ForceRankingConnection(boost::asio::io_service& io_service, ForceRankingServer * const server);

	double Eval(std::unique_ptr<QueryTree> tree);
	DocID doc_id;

    SocketStream ranking_stream;

	ForceRankingServer* const server;
};

#endif
