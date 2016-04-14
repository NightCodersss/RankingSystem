#include "streams_dispatcher.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/timer/timer.hpp>

StreamsDispatcher::StreamsDispatcher(
		std::vector<double>* rank_linear_form, 
		std::map<TextID, TextIndex>* index_by_id, 
		std::vector<TextID>* id_by_index,
		const config_type * const config)
	: rank_linear_form(rank_linear_form)
	, index_by_id(index_by_id)
	, id_by_index(id_by_index)
	, config(config)
{
}

void StreamsDispatcher::parse(const SouthRequest& south_request)
{
	rank_linear_form->clear();
	if (south_request.is_request_atomic) // Requests for index server 
	{
		int text_index = 0;
		for (auto const& text: (*config)["texts"])
		{
			TextID text_id = text["text_id"].get<std::string>();

			NorthRequest request;
			request.type = NorthRequest::Type::Index;
			//request.query = south_request.query;
			request.query = south_request.query_tree->packToQuery();
			request.text_index = text_index;
			request.text_id = text_id;

			rank_linear_form->push_back(text["factor"].get<double>());
			index_by_id->insert({text_id, text_index});
			id_by_index->push_back(text_id);
			
			requests.push_back(request);
			text_index++;
		}
	}
	else // request is a composite
	{
		int number_of_positive_childrens = 0;
		for (auto const& sub_query: south_request.query_tree->children) 
		{
			if (sub_query->op != QueryOperator::Not) 
				++number_of_positive_childrens;
		}
		double linear_form_coeff = 1./number_of_positive_childrens;
		double linear_form_coeff_negative = south_request.query_tree->children.size() != number_of_positive_childrens ? 
			-1. / (south_request.query_tree->children.size() - number_of_positive_childrens) :
			0.;
		BOOST_LOG_TRIVIAL(trace) << "Linear_form_coeff: " << linear_form_coeff <<"; negate: " << linear_form_coeff_negative;
		int text_index = 0;
		for (auto const& sub_query: south_request.query_tree->children)
		{
			NorthRequest request;
			request.type = NorthRequest::Type::Ranking;
			request.text_index = text_index;
			request.text_id = std::string("operation TextID: ") + std::to_string(text_index);
			if (sub_query->op != QueryOperator::Not) 
			{
				request.query = sub_query->packToQuery();
				rank_linear_form->push_back(linear_form_coeff);
			}
			else
			{
				request.query = sub_query->children[0]->packToQuery();
				rank_linear_form->push_back(linear_form_coeff_negative);
			}

			// index_id are meaningless if request is composite
			requests.push_back(request);
			text_index++;
		}
	}
}
