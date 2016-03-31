#pragma once

#include <memory>
#include "query.hpp"
#include "querytree.hpp"

class QueryParser
{
public:
	QueryParser() = default;
	std::unique_ptr<QueryTree> parse(const Query& query);

private:
	std::unique_ptr<QueryTree> parse_tree();
	std::unique_ptr<QueryTree> parse_or();
	std::unique_ptr<QueryTree> parse_and();
	std::unique_ptr<QueryTree> parse_atom();

	Query query = Query("");
	std::vector<std::string> tokens;
	int pos;
};
