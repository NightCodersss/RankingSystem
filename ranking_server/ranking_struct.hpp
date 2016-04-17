#ifndef RANKING_STRUCT_HPP
#define RANKING_STRUCT_HPP

#include <thread>
#include <future>
#include <string>
#include <set>
#include "sortbyrankgetbyidwithtop.hpp"
#include <document_accumulator.hpp>
#include <defines.hpp>

struct RankingStruct
{
	RankingStruct() = default;
	RankingStruct(config_type const& config);

	void update_min_for_text(TextIndex text_index, double new_val);

	void insertText(Document doc, TextIndex text_index);
	ubjson::Value formAnswer(DocID doc_id, double rank);
	void updateCuttingConsts(long long amount, double tmpMdr);
    bool isTheTopDocGoodEnough(config_type const& config, double max_swap_probability, int check_size);
    void deleteTheTopDocument();
	ubjson::Value forceUpdate(DocID doc_id, SocketStream& force_ranking_stream, std::string& query);
	ubjson::Value forwardQuery(DocID doc_id, std::string& query);

	std::string docTableToString();

	bool needsRefreshingRanksInMap = false; // enabled if there is negative values - in this case rank_lower_bound may be updated not only at inserting text
	void RefreshRanksInMap();

private: 
	double calculatePairSwapProbability(double x1, double dx1, double x2, double dx2);

public:
	std::map<DocID, DocumentAccumulator> docs; // docid, doc
	SortByRankGetByIdWithTop<DocID, double> docs_top {-10, -10}; // TODO set top_const, bottom_const
	std::mutex docs_mutex;

	bool is_end = false; // NOTE: maybe use std::atomic_flag
	int download_counter = 0;
	std::mutex download_counter_mutex;
	double Mdr;

	std::vector<double> rank_linear_form;

	std::vector<double> min_for_text;
	std::mutex min_for_text_mutex;

	RankFormPolicity rank_form_policity;

	std::set<DocID> deleted;
	std::mutex deleted_mutex;

	std::map<TextID, TextIndex> index_by_id;
	std::vector<TextID> id_by_index;
};

#endif
