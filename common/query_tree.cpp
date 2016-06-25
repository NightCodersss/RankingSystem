#include "query_tree.hpp"
#include "query_parser.hpp"

QueryTree::QueryTree(QueryOperator op, std::vector<std::unique_ptr<QueryTree>> children) 
	: op(op)
	, children(std::move(children))
{
}
	
QueryTree::QueryTree(std::string word) 
	: op(QueryOperator::Leaf)
	, word(word) 
{
} 

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

	if (!isAtom()) {
		query = children[0]->packToQuery().getText();

		for (auto it = std::begin(children) + 1; it != std::end(children); ++it) {
			const auto& c = *it;
			query += delim + c->packToQuery().getText();
		}
	} else {
		return word;	
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
	return op == QueryOperator::Leaf; 
}

std::string QueryTree::toString() const
{
	if (isAtom()) {
		return word;
	}

	std::string operation;
	switch (op) {
		case QueryOperator::Not: operation = "not"; break;
		case QueryOperator::And: operation = "and"; break;
		case QueryOperator::Or: operation = "or"; break;		
		case QueryOperator::And2: operation = "and2"; break;
		default: break;
	}

	std::string query = "(" + operation;
	for (const auto& c: children) {
		query += " " + c->toString();
	}
	query += ")";
	return query;
}
