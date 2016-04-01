#include "streams_dispatcher.hpp"

StreamsDispatcher::StreamsDispatcher(
		std::vector<double>* rank_linear_form, 
		std::map<TextID, TextIndex>* index_by_id, 
		std::vector<TextID>* id_by_index)
	: rank_linear_form(rank_linear_form)
	, index_by_id(index_by_id)
	, id_by_index(id_by_index)
{
}

void StreamsDispatcher::parse(SouthRequest south_request)
{
	// TODO set linear form
	if (south_request.is_request_atomic) // Reuqests for index server 
	{
		int text_index = 0;
		for (auto const& text: texts)
		{
			NorthRequest request;
			request.type = Request::Type::Index;
			request.query = query;
			request.text_index = text_index;
			request.text_id = text["text_id"].get<std::string>();

			requests.push_back(request);
			text_index++;
		}
	}
	else // request is a composite
	{
		int text_index = 0;
		for (auto const& sub_query: query_tree->children)
		{
			NorthRequest request;
			request.type = Request::Type::Ranking;
			request.query = sub_query->packToQuery();
			request.text_index = text_index;
			request.text_id = std::string("operation TextID: ") + std::to_string(text_index);

			requests.push_back(request);
			text_index++;
		}
	}
}
