#include "document.hpp"
#include <sstream>

Document::Document(DocID doc_id, double rank)
	: doc_id(doc_id)
	, rank(rank)
{
}

ubjson::Value Document::packToUbjson()
{
	ubjson::Value doc_ubjson;
	doc_ubjson["doc_id"] = doc_id;
	doc_ubjson["rank"] = rank;
	return doc_ubjson;
}

Document Document::unpackFromUbjson(const ubjson::Value& doc_ubjson)
{
	try {
		return Document(static_cast<const DocID&>(doc_ubjson["doc_id"])
					  , static_cast<double>      (doc_ubjson["rank"]));
	} catch (const std::exception& e) {
		std::stringstream ss;
		ss << ubjson::to_ostream(doc_ubjson);
		throw std::logic_error(std::string("wrong format of incoming ubjson document: ") + e.what() + "; doc: " + ss.str());
	}
}

std::string Document::toString() const
{
	std::stringstream ss;
	ss << "Document: {doc_id: " << doc_id << "; rank: " << rank << "}";
	return ss.str(); 
}
