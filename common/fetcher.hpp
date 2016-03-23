#pragma once

#include <document.hpp>

class Fetcher 
{
public:
	struct Iterator 
	{
		friend class Fetcher;

		void operator ++ ();
		Document operator * ();
		bool operator == (const Iterator& other);
	private:
		Iterator(Fetcher* fetcher);
	
	private:
		bool is_end = false;
		Document doc;
		Fetcher* fetcher;
	};
public:
	Fetcher(SocketStream& stream);
	boost::optional<Document> fetch();
	Iterator begin();
	Iterator end();
private:
	ubjson::StreamReader<SocketStream> reader;
};
