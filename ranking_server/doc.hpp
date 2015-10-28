#ifndef DOC_HPP
#define DOC_HPP

#include <bitset>
#include <map>
#include <config_loader.hpp>
#include <value.hpp>

using DocID = long long;
using TextID = std::string;
		
using TextIndex = int;

struct Doc
{
	static const int TextCount = 10;

	Doc(const ubjson::Value& d);
	Doc() = default;

	ubjson::Value doc;
	std::bitset<TextCount> got = { };
	double mdr = 1;
	void update(json const& config, std::map<TextID, double> const& c); // TODO cut this shit out
};

#endif