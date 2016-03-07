#ifndef TEXT_INDEX_INFO_HPP
#define TEXT_INDEX_INFO_HPP

#include <string>
#include <defines.hpp>

struct TextIndexInfo
{
	TextIndexInfo(DocID doc_id, double correspondence);

	DocID doc_id;
	double correspondence;
};

#endif
