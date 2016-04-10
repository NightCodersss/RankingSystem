#ifndef RANKING_STRUCT_HPP
#define RANKING_STRUCT_HPP

#include <thread>
#include <future>
#include <string>
#include "sortbyrankgetbyidwithtop.hpp"
#include "document_accumulator.hpp"
#include <defines.hpp>

struct RankingStruct
{
	RankingStruct() = default;
	RankingStruct(config_type const& config);

	void update_min_for_text(TextIndex text_index, double new_val);

	void insertText(Document doc, TextIndex text_index);
	ubjson::Value formAnswer();
	void updateCuttingConsts(long long amount, double tmpMdr);
    bool isTheTopDocGoodEnough(config_type const& config, double max_swap_probability, int check_size);
    void deleteTheTopDocument();

	std::string docTableToString();

private: 
	double calculatePairSwapProbability(double x1, double dx1, double x2, double dx2);

public:
	std::map<DocID, DocumentAccumulator> docs; // docid, doc
	SortByRankGetByIdWithTop<DocID, double> docs_top {0, 0}; // TODO set top_const, bottom_const
	std::mutex docs_mutex;

	bool is_end = false; // NOTE: maybe use std::atomic_flag
	int download_counter = 0;
	double Mdr;

	std::vector<double> rank_linear_form;
	std::vector<double> min_for_text;
	RankFormPolicity rank_form_policity;

	std::set<DocID> deleted;

	std::map<TextID, TextIndex> index_by_id;
	std::vector<TextID> id_by_index;
};

#endif
