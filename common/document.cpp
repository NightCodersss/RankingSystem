#include "document.hpp"

Document::Document(DocID doc_id, double correspondence)
	: doc_id(doc_id)
	, correspondence(correspondence)
{
}

ubjson::Value Document::packToUbjson()
{
	ubjson::Value doc_ubjson;
	doc_ubjson["docid"] = doc_id;
	doc_ubjson["correspondence"] = correspondence;
	return doc_ubjson;
}
