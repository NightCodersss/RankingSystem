#ifndef DOC_HPP
#define DOC_HPP

#include <bitset>
#include <string>
#include <map>
#include <config_loader.hpp>
#include <value.hpp>
#include <document.hpp>
#include <defines.hpp>

enum class RankFormPolicity { Max, Sum, MultipliedSum, NotImplemented};

struct DocumentAccumulator
{
	static const int DocCount = 500; // WARN
	DocumentAccumulator(DocID doc_id = 0, const std::vector<double> * rank_linear_form = nullptr, RankFormPolicity rank_form_policity = RankFormPolicity::Sum);

	Document aggregate();
	double mdr(const std::vector<double>& min_for_text);
	double rank_lower_bound(const std::vector<double>& min_for_text);
	double rank_upper_bound(const std::vector<double>& min_for_text);
	void addDocument(const Document& doc, TextIndex text_index);

	std::string toString(const std::vector<double>& min_for_text);

	RankFormPolicity rank_form_policity;
	std::bitset<DocCount> got = { };
	DocID doc_id;
	const std::vector<double> * rank_linear_form;
private:
	double rank = 0; // hidden to avoid misunderstang of rank. It's partly calculated rank. You can get bound by upper_bound and lower_bound methods. This is system variable, that can be not between the upper and lower bounds.
	double get_rank(); // returns rank of document, if it's all information we got. It's correct to call this iff all the texts are done.
	int number_of_positive_texts;
	int number_of_got_positive_texts = 0;
};

#endif
