#include <iostream>
#include <cstdint>
#include "storage.hpp"
#include "serialize.hpp"
#include "serializer.hpp"
#include "keyvaluestorage.hpp"
#include "bigstorage.hpp"

int main()
{
//	auto storage = make_storage<std::pair<std::uint32_t, std::uint32_t>>("forward_storage.bin", pairSerializer());
//	auto storage = makeKeyValueStorage<std::uint32_t, std::uint32_t>("forward_storage.bin", pairSerializer());
	
	auto storage = BigStorage("index");

	Record record;
	Commit commit;

	while (std::cin >> record.word >> record.text_id >> record.doc_id >> record.rank) {
		commit.push_back(record);
	}

	storage.addCommit(commit);

	//storage.dumpStorage();
}

