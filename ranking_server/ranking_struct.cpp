
#include "ranking_struct.hpp"

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
		docs[docid].got[text_index] = 1;
		docs_top.increment(docid, delta);
	}
}

ubjson::Value RankingStruct::formAnswer(long long amount)
{
	ubjson::Value answer;

	long long res_size = 0;

	for(auto doc_it = docs_top.top_begin(); doc_it != docs_top.top_end(); ++doc_it)
	{
		const auto& doc = *doc_it;
		docs[doc.second].doc["rank"] = doc.first;
		answer["docs"].push_back(docs[doc.second].doc);
		++res_size;
		if (amount != -1 && res_size >= amount)//TODO: add sup limit
 			break;
	}
	answer["amount"] = res_size;
	return answer;
}

void RankingStruct::updateRankingConsts(long long amount, double Mdr_copy)
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

double RankingStruct::computeSwapProbability(config_type const& config)
{
	std::lock_guard<std::mutex> lock(docs_mutex);

	// TODO: refactor 3 lines: make it simpler
	auto end = docs_top.top_end(); // End of docs_top.top
	--end; //Last of docs_top.top
	auto lastAndOne = docs_top.all_upper_bound(end -> first); // prev elem to (last in top) in docs_top.all 

	double swap_prob = 0;

	// TODO: refactor: 
	auto it = docs_top.all_begin();
	for ( ; it != lastAndOne; ++it ) // *it is (rank_of_doc, doc_id)
	{
		docs[it -> second].update(config, c);
	}
	docs[it -> second].update(config, c);

	for (it = docs_top.all_begin(); it != lastAndOne; ++it) // *it is (rank_of_doc, doc_id)
	{
		double x1 = it -> first;					
		double dx1 = docs[it -> second].mdr;

		auto next = it;
		++next; 

		double x2 = next -> first;
		double dx2 = docs[next -> second].mdr;

		swap_prob += calculatePairSwapProbability(x1, dx1, x2, dx2);
	}

	return swap_prob;
}
