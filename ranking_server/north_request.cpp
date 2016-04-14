#include "north_request.hpp"

ubjson::Value NorthRequest::formUbjsonRequest()
{
	ubjson::Value request;
	request["query"] = query.getText();
	request["fields"] = {"docname", "url", "docid"};
	request["index_id"] = text_id;
	request["nonroot"] = "yes"; // is used only by ranking server, so it is always non-root
	return request;
}
