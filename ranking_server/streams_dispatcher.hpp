#pragma once

#include "north_request.hpp"
#include "south_request.hpp"
#include <defines.hpp>
#include <config_loader.hpp> // for config_type

class StreamsDispatcher
{
public:
	StreamsDispatcher(
		std::vector<double>* rank_linear_form, 
		std::map<TextID, TextIndex>* index_by_id, 
		std::vector<TextID>* id_by_index,
		const config_type * const config);

	void parse(const SouthRequest& south_request);
	std::vector<NorthRequest> requests;

private:
	std::vector<double>* rank_linear_form;
	std::map<TextID, TextIndex>* index_by_id;
	std::vector<TextID>* id_by_index;
	const config_type* const config;
	
};
