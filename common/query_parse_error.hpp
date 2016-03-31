#pragma once

#include <stdexcept>
#include "query.hpp"

class QueryParseError : public std::invalid_argument
{
public:
	QueryParseError(const Query& query, int pos, const std::string& explanation);	
	const Query& getQuery() const;

private:
	Query query;
};
