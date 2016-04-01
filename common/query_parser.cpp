#include "query_parser.hpp"
#include "util.hpp"
#include "query_parse_error.hpp"
#include <cctype>

std::unique_ptr<QueryTree> QueryParser::parse(const Query& query)
{
	this->query = query;

	tokens.clear();
	
	auto text = query.getText();
	for (auto it = std::begin(text); it != std::end(text); ) {
		while (it != std::end(text) && std::isspace(*it)) {
			++it;
		}

		if (it == std::end(text)) {
			break;
		}

		switch (*it) {
			case '!': case '(': case ')': case '|':
				tokens.push_back(std::string("") + *it);
				++it;
				break;
			default:			
			{
				std::string word = "";
				while (! (it == std::end(text) 
					   || std::isspace(*it) 
					   || *it == '!' 
					   || *it == '(' 
					   || *it == ')' 
					   || *it == '|') ) {
					word += *it;
					++it;
				}

				tokens.push_back(word);
			}
		}
	}

	pos = 0;
	return parse_tree();
}

std::unique_ptr<QueryTree> QueryParser::parse_tree()
{
	return parse_or();
}

std::unique_ptr<QueryTree> QueryParser::parse_or()
{
	std::vector<std::unique_ptr<QueryTree>> ops;

	ops.push_back(parse_and());
	while (pos < tokens.size() && tokens[pos] == "|") {
		++pos;
		ops.push_back(parse_and());
	}

	return std::make_unique<QueryTree>(QueryOperator::Or, std::move(ops));
}

std::unique_ptr<QueryTree> QueryParser::parse_and()
{
	std::vector<std::unique_ptr<QueryTree>> ops;

	ops.push_back(parse_atom());
	while (pos < tokens.size() && tokens[pos] != "|") {
		++pos;
		ops.push_back(parse_atom());
	}

	return std::make_unique<QueryTree>(QueryOperator::And, std::move(ops));
}

std::unique_ptr<QueryTree> QueryParser::parse_atom()
{
	auto word = tokens[pos];
	++pos;

	if (word == "!") {
		std::vector<std::unique_ptr<QueryTree>> ops;
		ops.emplace_back(parse_atom());
		return std::make_unique<QueryTree>(QueryOperator::Not, std::move(ops));
	}
	else if (word == "(") {
		auto q = parse_tree();
		if (word != ")") {
			throw QueryParseError(query, pos, "Inbalanced parentheses");
		}
		++pos;
		return std::move(q);
	}
	else {
		return std::make_unique<QueryTree>(word);
	}
}
