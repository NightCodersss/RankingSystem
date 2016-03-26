#include "doc.hpp"
#include <sstream>

Doc::Doc(const Document& d) : doc(d) { }

void Doc::update(json const& config, std::map<TextID, double> const& c)
{
	int text_index = 0;
	mdr = 0;
	for ( const auto& text: config["texts"] )
	{
		auto text_id = text["index_id"].get<TextID>();
		mdr += c.at(text_id) * !got[text_index];
	}
}

std::string Doc::toString() const 
{
	std::stringstream ss;
	ss << doc.toString() << "\t" << "mdr: " << mdr << "\t" << "got: " << got.to_string('.', '+');
	return ss.str();
}
