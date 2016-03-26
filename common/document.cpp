#include "document.hpp"
#include <sstream>

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

Document Document::unpackFromUbjson(const ubjson::Value& doc_ubjson)
{
	try {
		return Document(static_cast<const DocID&>(doc_ubjson["docid"])
					  , static_cast<double>      (doc_ubjson["correspondence"]));
	} catch (const std::exception& e) {
		throw std::logic_error(std::string("wrong format of incoming ubjson document: ") + e.what());
	}
}

std::string Document::toString()
{
	std::stringstream ss;
	ss << "Document: {docid: " << doc_id << "; correspondence: " << correspondence << "}";
	return ss.str(); 
}
