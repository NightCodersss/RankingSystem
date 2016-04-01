#include "query_parse_error.hpp"

QueryParseError::QueryParseError(const Query& query, int pos, const std::string& explanation) 
	: std::invalid_argument(std::string("Error parsing query: ")
	                      + "<query> = \"" + query.getText() + "\"; " 
						  + "<pos> = " + std::to_string(pos) + "; " 
						  + "<explanation> = " + explanation)
	, query(query)						  
{

}
	
const Query& QueryParseError::getQuery() const { return query; }
