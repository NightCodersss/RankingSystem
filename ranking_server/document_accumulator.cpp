#include "document_accumulator.hpp"
#include <sstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

DocumentAccumulator::DocumentAccumulator(DocID doc_id, const std::vector<double>* rank_linear_form)
	: doc_id(doc_id)
	, rank_linear_form(rank_linear_form)
{
}

double DocumentAccumulator::mdr(const std::vector<double>& min_for_text)
{
	if (min_for_text.size() != rank_linear_form->size())
		throw std::logic_error("min_for_text.size() must be equal to rank_linear_form.size()");
	double mdr = 0;
	for ( TextIndex i = 0; i < min_for_text.size(); ++i)
	{
		mdr += !got[i] * min_for_text[i] * (*rank_linear_form)[i];
	}
	return mdr;
}

std::string DocumentAccumulator::toString(const std::vector<double>& min_for_text) 
{
	std::stringstream ss;
	ss << "doc_id: " << doc_id << "rank (doc_acc): " << rank << "\t" << "mdr: " << mdr(min_for_text) << "\t" << "got: " << got.to_string('.', '+');
	return ss.str();
}

Document DocumentAccumulator::aggregate()
{
	return Document(doc_id, rank);
}

void DocumentAccumulator::addDocument(const Document& doc, TextIndex text_index)
{
	if (!got[text_index])
	{
		BOOST_LOG_TRIVIAL(trace) << "Inserting doc with id: " << doc_id;

		got[text_index] = 1;
		double delta = rank_linear_form->at(text_index) * doc.rank;
		rank += delta;
	}
	else 
	{
		BOOST_LOG_TRIVIAL(trace) << "Ignoring inserting doc with id: " << doc_id << " because such text is already got";
	}
}
