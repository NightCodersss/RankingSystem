#ifndef DOC_HPP
#define DOC_HPP

#include <bitset>
#include <string>
#include <map>
#include <config_loader.hpp>
#include <value.hpp>
#include <document.hpp>
#include <defines.hpp>

enum class RankFormPolicity { Max, Sum, NotImplemented};

struct DocumentAccumulator
{
	static const int DocCount = 500; // WARN
	DocumentAccumulator(DocID doc_id = 0, const std::vector<double> * rank_linear_form = nullptr, RankFormPolicity rank_form_policity = RankFormPolicity::Sum);

	Document aggregate();
	double mdr(const std::vector<double>& min_for_text);
	void addDocument(const Document& doc, TextIndex text_index);

	std::string toString(const std::vector<double>& min_for_text);

	RankFormPolicity rank_form_policity;
	std::bitset<DocCount> got = { };
	double rank = 0;
	DocID doc_id;
	const std::vector<double> * rank_linear_form;
};

#endif
