#ifndef RANKING_STRUCT_HPP
#define RANKING_STRUCT_HPP

#include <thread>
#include <future>
#include "sortbyrankgetbyidwithtop.hpp"
#include "doc.hpp"

using DocID = long long;
using TextID = std::string;

using TextIndex = int;

struct RankingStruct
{
	RankingStruct() = default;
	RankingStruct(config_type const& config);

	void update_C(TextID text_id, double factor, double new_val); 
	void insertText(DocID docid, TextIndex text_index, const Doc& doc, double delta);
	ubjson::Value formAnswer();
	void updateCuttingConsts(long long amount, double tmpMdr);
    bool isTheTopDocGoodEnough(config_type const& config, double max_swap_probability, int check_size);
    void deleteTheTopDocument();

private: 
	double calculatePairSwapProbability(double x1, double dx1, double x2, double dx2);

public:
	std::map<DocID, Doc> docs; // docid, doc
	SortByRankGetByIdWithTop<DocID, double> docs_top {0, 0}; // TODO set top_const, bottom_const
	std::mutex docs_mutex;
	bool is_end = false; // NOTE: maybe use std::atomic_flag
	int download_counter = 0;
	double Mdr;
    double last_rank = 1.;
	std::map<TextID, double> c;
};

#endif
