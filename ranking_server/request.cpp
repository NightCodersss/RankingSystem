#include "request.hpp"

ubjson::Value Request::formUbjsonReuqest(TextID text_id)
{
	ubjson::Value query;
	query["query"] = query.getText();
	query["fields"] = {"docname", "url", "docid"};
	query["index_id"] = text_id;
	query["nonroot"] = "yes"; // is used only by ranking server, so it is always non-root
	return query;
}
