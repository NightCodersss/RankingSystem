#include "south_request.hpp"
#include <cassert>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

void SouthRequest::parse(ubjson::Value request)
{
	// TODO non-root detector
	if (request["query"].isNull())
		throw std::logic_error("query is empty");
	amount = std::min(100, request["amount"].asInt());
	query = Query(request["query"]);
	query_tree = QueryParser().parse(query);
	BOOST_LOG_TRIVIAL(trace) << "QueryTree: " << query_tree->toString();
	is_request_atomic = query_tree->isAtom();
	query_operator = query_tree->op;
	is_root = request["nonroot"].isNull();
}

ubjson::Value SouthRequest::forwardQuery(DocID doc_id) // TODO move it from here
{
	if (!is_request_atomic) 
		throw std::logic_error("you need to ask forward server only on atomic queries");
	ubjson::Value res;
//	res["query"] = query.getText();
	res["query"] = query_tree->packToQuery().getText();
	res["doc_id"] = doc_id;
	return res;
}
