#ifndef TEXT_INDEX_INFO_HPP
#define TEXT_INDEX_INFO_HPP

#include <string>

struct TextIndexInfo
{
	std::string word;
	long long doc_id;
	double correspondence;
};

#endif
