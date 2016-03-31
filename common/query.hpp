#pragma once

#include <string>

class Query
{
public:
	Query(const std::string& text);
	Query(const std::string&& text);
	const std::string& getText() const;
private:
	std::string text;
};
