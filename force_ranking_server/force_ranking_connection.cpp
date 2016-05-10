#include <sstream>
#include <thread>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"

#include <query.hpp>
#include <query_tree.hpp>
#include <query_parser.hpp>
#include <query_parse_error.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

#include <document.hpp>

#include "force_ranking_server.hpp"
#include "force_ranking_connection.hpp"

#include <defines.hpp>

using boost::asio::ip::tcp;

ForceRankingConnection::pointer ForceRankingConnection::create(boost::asio::io_service& io_service, ForceRankingServer* const server)
{
    return pointer(new ForceRankingConnection(io_service, server));
} 
    
void ForceRankingConnection::start()
{
	server->inc_connections();
	std::thread([self = shared_from_this()]() {
		ubjson::StreamReader<SocketStream> reader(self->ranking_stream);
		auto request = reader.getNextValue();

		// BOOST_LOG_TRIVIAL(trace) << "Read json: " << ubjson::to_ostream(request) << '\n';
		BOOST_LOG_TRIVIAL(trace) << "Read json.";
	//	std::cout << "ForceRanking: Read json: " << ubjson::to_ostream(request) << '\n';

		if(request["query"].isNull())
			return;

		Query query(request["query"]);
		auto query_tree = QueryParser().parse(query);
		self->doc_id = request["doc_id"].asInt();
		BOOST_LOG_TRIVIAL(trace) << "Doc id: " << self->doc_id << "\n";

		ubjson::StreamWriter<SocketStream> writer(self->ranking_stream);
		ubjson::Value result;

		double rank = self->Eval(std::move(query_tree));
		result["doc_id"] = static_cast<UbjsonDocID>(self->doc_id);
		result["rank"] = rank; 
		BOOST_LOG_TRIVIAL(trace) << "Result: doc_id: " << self->doc_id << ", rank: " << rank <<", query: " << query.getText() << "\n";

		BOOST_LOG_TRIVIAL(trace) << "Writing output to ranking server\n";
		writer.writeValue(result);
		BOOST_LOG_TRIVIAL(trace) << "Wrote output to ranking server\n";
		const auto& err = self->ranking_stream.error();  
		if ( err )
		{
			BOOST_LOG_TRIVIAL(info) << "Error while writing: " << err.message();
		}
		BOOST_LOG_TRIVIAL(trace) << "Leaving main lambda function. Resourses going to be freed.";
	}).detach();
}
    
ForceRankingConnection::ForceRankingConnection(boost::asio::io_service& io_service, ForceRankingServer* const server) : server(server) { }

ForceRankingConnection::~ForceRankingConnection()
{
	BOOST_LOG_TRIVIAL(trace) << "Destructor of connection in.";
	server->dec_connections();
}

double ForceRankingConnection::Eval(std::unique_ptr<QueryTree> tree) {
	if (tree->isAtom()) // Fetch from forward index server
	{
		int server_index = 0;				

		SocketStream forward_index_stream(server->config["servers"]["forward"][server_index]["host"].get<std::string>()
				, server->config["servers"]["forward"][server_index]["port"].get<std::string>());
		ubjson::StreamWriter<SocketStream> writer(forward_index_stream);

		writer.writeValue(forwardQuery(tree->packToQuery().getText()));

		ubjson::StreamReader<SocketStream> reader(forward_index_stream);
		auto answer = reader.getNextValue();

		double res = 0;
		for (const auto& doc: answer) {
			auto text_id = static_cast<std::string>(doc["text_id"]);
			auto d = Document::unpackFromUbjson(doc);
			res += d.rank * server->rank_form_by_id.at(text_id);
		}
		return res;
	}
	else
	{
		if (tree->op == QueryOperator::And) 
		{
			BOOST_LOG_TRIVIAL(trace) << "Parse and";
			double res = 0;
			int number_of_positive = 0;
			for (int i = 0; i < tree->children.size(); ++i) {
				if (tree->children[i]->op != QueryOperator::Not) {
					++number_of_positive;
				}
			}
			double positive_factor = 1./number_of_positive;
			double negative_factor = tree->children.size() != number_of_positive ? 
				1./(tree->children.size() - number_of_positive) :
				0.;
			for (int i = 0; i < tree->children.size(); ++i) {
				double e = Eval(std::move(tree->children[i]));
				res +=  e > 0 ? e * positive_factor : e * negative_factor; // minus for negative factor became from "not" case
			}
			return res;
		}
		else if (tree->op == QueryOperator::Or) {
			BOOST_LOG_TRIVIAL(trace) << "Parse or";
			double res = 0;
			for (int i = 0; i < tree->children.size(); ++i) {
				res = std::max(res, Eval(std::move(tree->children[i])));
			}
			return res;
		}
		else if (tree->op == QueryOperator::Not) {
			BOOST_LOG_TRIVIAL(trace) << "Parse not";
			return -Eval(std::move(tree->children[0]));
		}
		else
			throw std::logic_error("Operator is not implemented yet");
	}
}

ubjson::Value ForceRankingConnection::forwardQuery(std::string word) 
{
	ubjson::Value res;
	res["query"] = word; 
	res["doc_id"] = static_cast<UbjsonDocID>(doc_id);
	return res;
}
