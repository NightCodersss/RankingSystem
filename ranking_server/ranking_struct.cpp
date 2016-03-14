#include "ranking_struct.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

RankingStruct::RankingStruct(config_type const& config)
{
	//Sum of 𝛎's
	Mdr = 0.;

	for ( const auto& text: config["texts"] )
	{
		auto text_id = text["index_id"].get<TextID>();
		c[text_id] = 1.;
		Mdr += text["factor"].get<double>();
	}

}
void RankingStruct::update_C(TextID text_id, double factor, double new_val)
{
	Mdr -= c[text_id] * factor;
	c[text_id] = new_val;
	Mdr += c[text_id] * factor;
}
		
void RankingStruct::insertText(DocID docid, TextIndex text_index, const Doc& doc, double delta)
{
	/**
	* It inserts text of one document to datastruct. 
	* Used at getting documetns from index_server.
	*/
	if(docs.find(docid) == docs.end())
	{
		docs[docid] = doc;
	}
	if (!docs[docid].got[text_index])
	{
		BOOST_LOG_TRIVIAL(trace) << "Inserting doc with id: " << docid;

		docs[docid].got[text_index] = 1;
		docs_top.increment(docid, delta);
	}
}

ubjson::Value RankingStruct::formAnswer()
{
    const auto& doc = *docs_top.top_begin();
    docs[doc.second].doc["rank"] = doc.first;
	return docs[doc.second].doc;
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
		
	return p;
}

bool RankingStruct::isTheTopDocGoodEnough(config_type const& config, double max_swap_probability, int check_size)
{
	std::lock_guard<std::mutex> lock(docs_mutex);

	if ( docs_top.all_begin() == docs_top.all_end() ) {
		return false;
	}

    auto the_top_document = docs_top.all_begin();
    docs[the_top_document -> second].update(config, c);
    double rank = the_top_document -> first;					
    double d_rank = docs[the_top_document -> second].mdr;

    // Check document that we have not seen
    if (calculatePairSwapProbability(rank, d_rank, 0, Mdr) > max_swap_probability)
        return false;

    // Check documents that we have already seen
    auto it = docs_top.all_begin();
    ++it;
	for (int counter = 0; it != docs_top.all_end() && counter < check_size; ++it) // *it is (rank_of_doc, doc_id)
	{
        docs[it -> second].update(config, c);
		double current_rank = it -> first;
		double d_current_rank = docs[it -> second].mdr;

		if (calculatePairSwapProbability(rank, d_rank, current_rank, d_current_rank) > max_swap_probability)
            return false;
	}

	return true;
}
