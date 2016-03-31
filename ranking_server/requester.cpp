#include "requester.hpp"

Requester::Requester(ubjson::Value request)
{
	// TODO non-root detcter
	if(request["query"].isNull())
		throw std::logic_error("query is empty");
	amount = std::min(100, request["amount"].asInt());
	query = Query(request["query"]);
	query_tree = QueryParser().parse(query);
	is_request_atomic = query_tree->isAtom();

	// Generating requests
	if (is_request_atomic) // Reuqests for index server 
	{
		int text_index = 0;
		for (auto const& text: texts)
		{
			Request request;
			request.type = Request::Type::Index;
			request.query = query;
			request.text_index = text_index;

			requests.push_back(request);
			text_index++;
		}
	}
	else // request is a composite
	{
		int text_index = 0;
		for (auto const& sub_query: query_tree->children)
		{
			Request request;
			request.type = Request::Type::Ranking;
			request.query = sub_query->packToQuery();
			request.text_index = text_index;

			requests.push_back(request);
			text_index++;
		}
	}
}

ubjson::Value Requester::forwardQuery(DocID doc_id)
{
	std::assert(is_request_atomic && "you need to ask forward server only on atomic queries");
	ubjson::Value res;
	res["query"] = query.getText();
	res["doc_id"] = doc_id;
	return res;
}
