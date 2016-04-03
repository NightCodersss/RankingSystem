#include "query_tree.hpp"
#include "query_parser.hpp"

QueryTree::QueryTree(QueryOperator op, std::vector<std::unique_ptr<QueryTree>> children) 
	: op(op)
	, children(std::move(children))
{
}
	
QueryTree::QueryTree(std::string word) : word(word) { } 

Query QueryTree::packToQuery() const
{
	// WARN TODO What if childrens are empty?
	std::string query;

	std::string delim;

	if (op == QueryOperator::Not) {
		return "! " + children[0]->packToQuery().getText();
	} 
		
	switch (op) {
		case QueryOperator::And: delim = " "; break;
		case QueryOperator::Or: delim = " | "; break;		
		default: break;
	}

	query = children[0]->packToQuery().getText();

	for (auto it = std::begin(children) + 1; it != std::end(children); ++it) {
		const auto& c = *it;
		query += delim + c->packToQuery().getText();
	}

	return query;
}

std::unique_ptr<QueryTree> unpackFromQuery(const Query& query)
{
	QueryParser parser;
	return parser.parse(query);
}

bool QueryTree::isAtom() const
{
	return children.size() == 0; // WARN TODO: it is not correct. It is a correct tree: or ( a, or ( _nothing_ ) )
}
