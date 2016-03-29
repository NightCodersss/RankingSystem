#ifndef DOC_HPP
#define DOC_HPP

#include <bitset>
#include <string>
#include <map>
#include <config_loader.hpp>
#include <value.hpp>
#include <document.hpp>
#include <defines.hpp>


struct DocumentAccumulator
{
	static const int DocCount = 500; // WARN
	DocumentAccumulator(DocID doc_id = 0, const std::vector<double> * rank_linear_form = nullptr);

	Document aggregate();
	double mdr(const std::vector<double>& min_for_text);
	void addDocument(const Document& doc, TextIndex text_index);

	std::string toString(const std::vector<double>& min_for_text);
	
	std::bitset<DocCount> got = { };
	double rank = 0;
	DocID doc_id;
	const std::vector<double> * rank_linear_form;
};

#endif
