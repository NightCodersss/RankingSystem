#include <fetcher.hpp>

Fetcher::Fetcher(SocketStream& stream) 
	: reader(stream)
{
}

boost::optional<Document> Fetcher::fetch()
{
		ubjson::Value doc_ubjson = reader.getNextValue();
		if(static_cast<int>(doc_ubjson["is_end"]) == 1)
			return boost::none;
		return Document(static_cast<const DocID&>(doc_ubjson["docid"])
					  , static_cast<double>      (doc_ubjson["correspondence"]));
}

Fetcher::Iterator Fetcher::begin()
{
	auto maybe_doc = fetch();
	if (maybe_doc) 
	{
		Iterator it(this);	
		it.doc = *maybe_doc;
		return it;
	}
	else
	{
		return end();
	}
}

Fetcher::Iterator Fetcher::end()
{
	Iterator it(this);
	it.is_end = true;
	return it;
}

Fetcher::Iterator::Iterator(Fetcher* fetcher)
	: fetcher(fetcher)
{
}

void Fetcher::Iterator::operator ++ () 
{
	*this = fetcher->begin();
}

Document Fetcher::Itarator::operator * ()
{
	if (is_end)
			throw std::out_of_range("");
	return doc;
}

bool Fetcher::Iterator::operator == (const Fetcher::Iterator& other)
{
	return is_end && other.is_end;
}
