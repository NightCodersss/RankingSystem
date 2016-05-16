#include <iostream>
#include <cstdint>
#include "storage.hpp"
#include "serialize.hpp"
#include "serializer.hpp"
#include "keyvaluestorage.hpp"
#include "bigstorage.hpp"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

int main()
{
//	auto storage = make_storage<std::pair<std::uint32_t, std::uint32_t>>("forward_storage.bin", pairSerializer());
//	auto storage = makeKeyValueStorage<std::uint32_t, std::uint32_t>("forward_storage.bin", pairSerializer());
	
	auto storage = BigStorage("index");

	Record record;
	Commit commit;

	std::string s;
	while (std::getline(std::cin, s))
	{
		std::vector<std::string> parts;
		boost::split(parts, s, boost::is_any_of(" "));

		record.word = parts.at(0);
		record.text_id = parts.at(1);
		record.doc_id = std::stoull(parts.at(2));
		record.rank = std::stod(parts.at(3));
		commit.push_back(record);
	}

	storage.addCommit(commit);

	//storage.dumpStorage();
}

