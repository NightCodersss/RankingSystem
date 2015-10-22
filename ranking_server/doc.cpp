#include "doc.hpp"

Doc::Doc(const ubjson::Value& d) : doc(d) { }

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

