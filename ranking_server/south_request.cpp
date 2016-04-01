#include "south_request.hpp"

void SouthRequest::parse(ubjson::Value request)
{
	// TODO non-root detcter
	if(request["query"].isNull())
		throw std::logic_error("query is empty");
	amount = std::min(100, request["amount"].asInt());
	query = Query(request["query"]);
	query_tree = QueryParser().parse(query);
	is_request_atomic = query_tree->isAtom();
	query_operator = query_tree->op;
}

ubjson::Value Requester::forwardQuery(DocID doc_id) // TODO move it from here
{
	std::assert(is_request_atomic && "you need to ask forward server only on atomic queries");
	ubjson::Value res;
	res["query"] = query.getText();
	res["doc_id"] = doc_id;
	return res;
}
