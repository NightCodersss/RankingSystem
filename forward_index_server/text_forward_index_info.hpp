#ifndef TEXT_FORWARD_INDEX_INFO_HPP
#define TEXT_FORWARD_INDEX_INFO_HPP

#include <string>
#include <defines.hpp>

struct TextForwardIndexInfo
{
	std::string word;
	DocID doc_id;
	double correspondence;
	TextID text_id;	
};

#endif
