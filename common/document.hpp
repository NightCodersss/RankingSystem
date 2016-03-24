#pragma once

#include <defines.hpp>
#include <value.hpp>

struct Document
{
	Document(DocID doc_id = 0, double correspondence = 0);
	ubjson::Value packToUbjson();
	DocID doc_id;
	double correspondence;
};
