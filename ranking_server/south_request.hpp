#pragma once

#include <query.hpp>
#include <query_tree.hpp>
#include <query_parser.hpp>
#include <query_parse_error.hpp>

class SouthRequest
{
public:
	Requester();
	void parseSouthRequest(ubjson::Value request);

	ubjson::Value forwardQuery(DocID doc_id);

	bool is_root = true;
	bool is_request_atomic;
	int amount;
	Query query;
	QueryOperator query_operator;
private:
	std::unique_ptr<QueryTree> query_tree;
};
