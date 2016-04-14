#pragma once
#include <value.hpp>
#include <query.hpp>
#include <defines.hpp>

class NorthRequest
{
public:
	enum class Type { Ranking, Index };

	ubjson::Value formUbjsonRequest();
	Type type;
	Query query;
	TextIndex text_index;
	TextID text_id;
};
