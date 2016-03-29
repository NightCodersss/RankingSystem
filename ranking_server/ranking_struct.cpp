#include "ranking_struct.hpp"

#include <sstream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

RankingStruct::RankingStruct(config_type const& config)
{
	//Sum of 𝛎's
	Mdr = 0.;

	TextIndex text_index = 0;

	for ( const auto& text: config["texts"] )
	{
		auto text_id = text["index_id"].get<TextID>();

		index_by_id[text_id] = text_index;
		++text_index;

		id_by_index.push_back(text_id);

		rank_linear_form.push_back(text["factor"].get<double>());
		Mdr += rank_linear_form.back();
	    min_for_text.push_back(1.);
	}

}
void RankingStruct::update_min_for_text(TextIndex text_index, double new_val)
{
	Mdr -= min_for_text[text_index] * rank_linear_form[text_index];
	min_for_text[text_index] = std::min(new_val, min_for_text[text_index]);
	Mdr += min_for_text[text_index] * rank_linear_form[text_index];
}
		
void RankingStruct::insertText(Document doc, TextIndex text_index)
{
	/**
	* It inserts text of one document to datastruct. 
	* Used at getting documetns from index_server.
	*/
	if(docs.find(doc.doc_id) == docs.end())
	{
		docs.emplace(doc.doc_id, DocumentAccumulator(doc.doc_id, &rank_linear_form));
	}

	docs[doc.doc_id].addDocument(doc, text_index/*, delta*/);

	double delta = rank_linear_form.at(text_index) * doc.rank;
	docs_top.increment(doc.doc_id, delta); // TODO replace `increment` to `update` without delta, but `with docs[...].rank`. Now it is inconsistent to calc rank of doc in DocumentAggregator and at docs_top.increment.
}

ubjson::Value RankingStruct::formAnswer()
{
    const auto& doc = *docs_top.top_begin();
    return docs[doc.second].aggregate().packToUbjson();
}
    
void RankingStruct::deleteTheTopDocument()
{
    // TODO: actually delete
    const auto& doc = *docs_top.top_begin();
    DocID top_doc_id = doc.second;
    docs.erase(top_doc_id);
    docs_top.deleteTheTopDocument();
}

void RankingStruct::updateCuttingConsts(long long amount, double Mdr_copy)
{
	std::lock_guard<std::mutex> lock(docs_mutex);
	if (docs_top.top_size() >= amount)
	{
		auto last_in_top = docs_top.top_end();

		for (int i = 0; i < docs_top.top_size() - amount + 1; ++i)
		{			
			--last_in_top;
		}

		auto new_top_const = last_in_top -> first;
		docs_top.setTopConst(new_top_const);
		docs_top.setBottomConst(new_top_const - Mdr_copy);
		docs_top.cutOff();
	}
}

double RankingStruct::calculatePairSwapProbability(double x1, double dx1, double x2, double dx2)
{
	double p;

	double M = std::min(x1 + dx1, x2 + dx2);
	double m = std::max(x1, x2);

	const double eps = 1e-6;

	if(std::abs(dx1) > eps && std::abs(dx2) > eps)
		p = (x1 + dx1) * (M - m) / dx1 / dx2 - (M*M - m*m)/(2 * dx1 * dx2);
	else if (std::abs(dx1) < eps)
		p = 0;
	else 
		p = (x1 + dx1 - x2) / dx1;

	if (p < 0)
		p = 0;

	BOOST_LOG_TRIVIAL(trace) << "Swap pobability of pair: " << p;
		
	return p;
}

bool RankingStruct::isTheTopDocGoodEnough(config_type const& config, double max_swap_probability, int check_size)
{
	BOOST_LOG_TRIVIAL(trace) << "isTheTopDocGoodEnough before lock";
	std::lock_guard<std::mutex> lock(docs_mutex);
	BOOST_LOG_TRIVIAL(trace) << "isTheTopDocGoodEnough before in lock";

	if ( docs_top.all_begin() == docs_top.all_end() ) {
		return false;
	}

    auto the_top_document = docs_top.all_begin();
    double rank = the_top_document -> first;					
    double d_rank = docs[the_top_document -> second].mdr(min_for_text);

    // Check document that we have not seen
    if (calculatePairSwapProbability(rank, d_rank, 0, Mdr) > max_swap_probability)
        return false;

    // Check documents that we have already seen
    auto it = docs_top.all_begin();
    ++it;
	for (int counter = 0; it != docs_top.all_end() && counter < check_size; ++it) // *it is (rank_of_doc, doc_id)
	{
		double current_rank = it -> first;
		double d_current_rank = docs[it -> second].mdr(min_for_text);

		if (calculatePairSwapProbability(rank, d_rank, current_rank, d_current_rank) > max_swap_probability)
            return false;
	}

	return true;
}

std::string RankingStruct::docTableToString() 
{
	std::lock_guard<std::mutex> lock(docs_mutex);
	std::stringstream ss;
	int n = 0;    
	for (auto it = docs_top.all_begin(); it != docs_top.all_end(); ++it) // *it is (rank_of_doc, doc_id)
	{
		double current_rank = it -> first;
		DocumentAccumulator& doc = docs[it -> second];
		ss << "N: " << n << "; " << doc.toString(min_for_text) << "\t" << "rank (docs_top): " << current_rank <<"\n";
	}
	
	return ss.str();
}
