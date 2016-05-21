#include "idf.hpp"

#include <fstream>
#include <cstdlib>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

IDFHandler::IDFHandler()
{
	Load(std::getenv("IDF_PATH"));
}

void IDFHandler::Load(std::string file)
{
	BOOST_LOG_TRIVIAL(info) << "Loading…";
	std::string row;
	std::ifstream idf_file(file.c_str());

	while (std::getline(idf_file, row))
	{
		std::stringstream ss;
		ss << row;
		
		std::string word;
		double idf_of_word;
		int count; // number of docs we find this word in
		ss >> word >> idf_of_word >> count;
		idf[word] = idf_of_word;
	}
	BOOST_LOG_TRIVIAL(info) << "Loaded. Words: " << idf.size();
}

double IDFHandler::GetIDF(std::string word)
{
	try 
	{
		return idf.at(word);
	}
	catch(const std::exception& e)
	{
		BOOST_LOG_TRIVIAL(info) << "Error at getting IDF. Word: " << word << "; Error: " << e.what();
		return 1.0; // neutral idf // NOTE WARN may be incorrect
	}
}
