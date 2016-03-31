#pragma once

#include <memory>
#include <vector>
#include "query.hpp"

enum class QueryOperator { And, Or, Not };

class QueryTree
{
public:
	QueryTree(std::string word);
	QueryTree(QueryOperator op, std::vector<std::unique_ptr<QueryTree>> children);
	Query packToQuery() const;
	bool isAtom() const;

public:
	QueryOperator op;
	std::vector<std::unique_ptr<QueryTree>> children;

private:
	std::string word;
};

std::unique_ptr<QueryTree> unpackFromQuery(const Query& query);
