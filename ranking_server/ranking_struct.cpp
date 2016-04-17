#include "ranking_struct.hpp"

#include <sstream>

#include <stream_reader.hpp>
#include <stream_writer.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

RankingStruct::RankingStruct(config_type const& config)
	: Mdr(1.)
{
	BOOST_LOG_TRIVIAL(trace) << "Number of texts: " << config["texts"].size();
	// min_for_text resize is doing in ranking connection, correspond to size of requests generated by dispatcher
}

void RankingStruct::update_min_for_text(TextIndex text_index, double new_val)
{
	BOOST_LOG_TRIVIAL(trace) << "Before lock: " << __FUNCTION__;
	std::lock_guard<std::mutex> lock(min_for_text_mutex);
	BOOST_LOG_TRIVIAL(trace) << "After lock: " << __FUNCTION__;
	min_for_text[text_index] = std::min(new_val, min_for_text[text_index]);
	for (int i = 0; i < min_for_text.size(); ++i)
		BOOST_LOG_TRIVIAL(trace) << "min_for_text " << i <<" : " << min_for_text[i];

	Mdr = DocumentAccumulator(0 /*DocID*/, &rank_linear_form, rank_form_policity).rank_upper_bound(min_for_text); // Mdr is max possible increasing rank of new document
}
		
void RankingStruct::insertText(Document doc, TextIndex text_index)
{
	/**
	* It inserts text of one document to datastruct. 
	* Used at getting documetns from index_server.
	*/
	{
		BOOST_LOG_TRIVIAL(trace) << "Before lock: " << __FUNCTION__;
		std::lock_guard<std::mutex> lock(deleted_mutex);
		BOOST_LOG_TRIVIAL(trace) << "After lock: " << __FUNCTION__;
		if(deleted.find(doc.doc_id) != deleted.end())
		{
			BOOST_LOG_TRIVIAL(trace) << "Ignoring insert text for doc_id: " << doc.doc_id << " , because document is deleted (already done and sent)";
			return;
		}
	}
	BOOST_LOG_TRIVIAL(trace) << "Before lock 2: " << __FUNCTION__;
	std::lock_guard<std::mutex> lock(docs_mutex);
	BOOST_LOG_TRIVIAL(trace) << "After lock 2: " << __FUNCTION__;
	if(docs.find(doc.doc_id) == docs.end())
	{
		docs.emplace(doc.doc_id, DocumentAccumulator(doc.doc_id, &rank_linear_form, rank_form_policity));
	}

	docs[doc.doc_id].addDocument(doc, text_index);

	docs_top.set(doc.doc_id, docs[doc.doc_id].rank_lower_bound(min_for_text));
}

ubjson::Value RankingStruct::formAnswer(DocID doc_id, double rank)
{
    return Document(doc_id, rank).packToUbjson();
}
    
void RankingStruct::deleteTheTopDocument()
{
	DocID top_doc_id;
	{
		BOOST_LOG_TRIVIAL(trace) << "Before lock: " << __FUNCTION__;
		std::lock_guard<std::mutex> lock(docs_mutex);
		BOOST_LOG_TRIVIAL(trace) << "After lock: " << __FUNCTION__;
		const auto& doc = *docs_top.top_begin();
		top_doc_id = doc.second;
		docs.erase(top_doc_id);
		docs_top.deleteTheTopDocument();
	}
	{
		BOOST_LOG_TRIVIAL(trace) << "Before lock 2: " << __FUNCTION__;
		std::lock_guard<std::mutex> lock(deleted_mutex);
		BOOST_LOG_TRIVIAL(trace) << "After lock 2: " << __FUNCTION__;
		deleted.insert(top_doc_id);
	}
}

void RankingStruct::updateCuttingConsts(long long amount, double Mdr_copy)
{
	BOOST_LOG_TRIVIAL(trace) << "Before lock: " << __FUNCTION__;
	std::lock_guard<std::mutex> lock(docs_mutex);
	BOOST_LOG_TRIVIAL(trace) << "After lock: " << __FUNCTION__;
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
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(dx1, dx2);
	}
	// now x1 < x2
	double p;
	
	if ( x1 + dx1 <= x2 )
		p = 0;
	else 
		p = 1; // TODO change hard probolilities to normal

	BOOST_LOG_TRIVIAL(trace) << "Swap pobability of pair (" << x1 << ", " << x1 + dx1 << "; " << x2 << ", " << x2 + dx2 << "): " << p;
		
	return p;
}

bool RankingStruct::isTheTopDocGoodEnough(config_type const& config, double max_swap_probability, int check_size)
{
	BOOST_LOG_TRIVIAL(trace) << "isTheTopDocGoodEnough before lock";
	std::lock_guard<std::mutex> lock(docs_mutex);
	std::lock_guard<std::mutex> lock2(min_for_text_mutex);
	BOOST_LOG_TRIVIAL(trace) << "isTheTopDocGoodEnough before in lock";

	if ( docs_top.all_begin() == docs_top.all_end() ) {
		return false;
	}

    auto the_top_document = docs_top.all_begin();
    double rank = docs[the_top_document -> second].rank_lower_bound(min_for_text);
    double d_rank = docs[the_top_document -> second].mdr(min_for_text);

    // Check document that we have not seen
    if (calculatePairSwapProbability(rank, d_rank, 0, Mdr) > max_swap_probability)
        return false;

    // Check documents that we have already seen
    auto it = docs_top.all_begin();
    ++it;
	for (int counter = 0; it != docs_top.all_end() && counter < check_size; ++it) // *it is (rank_of_doc, doc_id)
	{
		double current_rank = docs[the_top_document -> second].rank_lower_bound(min_for_text);
		double d_current_rank = docs[it -> second].mdr(min_for_text);

		if (calculatePairSwapProbability(rank, d_rank, current_rank, d_current_rank) > max_swap_probability)
            return false;
	}

	return true;
}

std::string RankingStruct::docTableToString() 
{
	BOOST_LOG_TRIVIAL(trace) << "Before lock: " << __FUNCTION__;
	std::lock_guard<std::mutex> lock(docs_mutex);
	std::lock_guard<std::mutex> lock2(min_for_text_mutex);
	BOOST_LOG_TRIVIAL(trace) << "After lock: " << __FUNCTION__;

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

ubjson::Value RankingStruct::forceUpdate(DocID doc_id, SocketStream& force_ranking_stream, std::string& query)
{
	{
		ubjson::StreamWriter<SocketStream> writer(force_ranking_stream);
		writer.writeValue(forwardQuery(doc_id, query));
	}
	double rank;
	{
		ubjson::StreamReader<SocketStream> reader(force_ranking_stream);
		auto answer = reader.getNextValue();
		rank = static_cast<double>(answer["rank"]);
	}
	return formAnswer(doc_id, rank);
}

ubjson::Value RankingStruct::forwardQuery(DocID doc_id, std::string& query)
{
	ubjson::Value res;
//	res["query"] = query.getText();
	res["query"] = query;
	res["doc_id"] = doc_id;
	return res;
}
