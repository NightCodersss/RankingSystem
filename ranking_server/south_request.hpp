#pragma once

#include <value.hpp>
#include <defines.hpp>
#include <query.hpp>
#include <query_tree.hpp>
#include <query_parser.hpp>
#include <query_parse_error.hpp>

class SouthRequest
{
public:
	SouthRequest() = default;
	void parse(ubjson::Value request);


	bool is_root = true;
	bool is_request_atomic;
	int amount = 10; // standart amount
	Query query;
	QueryOperator query_operator;
	std::unique_ptr<QueryTree> query_tree;
};
