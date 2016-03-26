#ifndef DOC_HPP
#define DOC_HPP

#include <bitset>
#include <string>
#include <map>
#include <config_loader.hpp>
#include <value.hpp>
#include <document.hpp>
#include <defines.hpp>


struct Doc
{
	static const int TextCount = 500;

	Doc(const Document& d);
	Doc() = default;

	std::string toString() const;

	Document doc;
	std::bitset<TextCount> got = { };
	double mdr = 1;
	void update(json const& config, std::map<TextID, double> const& c); // TODO cut this shit out
};

#endif
