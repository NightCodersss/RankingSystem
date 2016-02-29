#ifndef TEXT_FORWARD_INDEX_INFO_HPP
#define TEXT_FORWARD_INDEX_INFO_HPP

#include <string>

struct TextForwardIndexInfo
{
	std::string word;
	long long doc_id;
	double correspondence;
};

#endif
