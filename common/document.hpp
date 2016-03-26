#pragma once

#include <string>
#include <defines.hpp>
#include <value.hpp>

struct Document
{
	Document(DocID doc_id = 0, double correspondence = 0);
	ubjson::Value packToUbjson();
	static Document unpackFromUbjson(const ubjson::Value& doc_json);
	
	std::string toString();

	DocID doc_id;
	double correspondence;
};
