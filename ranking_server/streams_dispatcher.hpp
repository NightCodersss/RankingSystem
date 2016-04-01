#include <north_request.hpp>

class StreamsDispatcher
{
	StreamsDispatcher(
		std::vector<double>* rank_linear_form, 
		std::map<TextID, TextIndex>* index_by_id, 
		std::vector<TextID>* id_by_index);
public:
	void parse(SouthRequest south_request);
	std::vector<NorthRequest> requests;

private:
	std::vector<double>* rank_linear_form;
	std::map<TextID, TextIndex>* index_by_id;
	std::vector<TextID>* id_by_index;
	
};
