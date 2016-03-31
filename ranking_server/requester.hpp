#include "request.hpp"
#include <query.hpp>
#include <query_parser.hpp>
#include <query_parser_error.hpp>

class Requester
{
public:
	Requester(ubjson::Value request);
	bool is_root = true;
	bool is_request_atomic;
	int amount;
	std::vector<Request> requests;
	Query query;
private:
	std::unique_ptr<QueryTree> query_tree;
};
