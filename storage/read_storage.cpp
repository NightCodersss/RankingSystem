#include <iostream>
#include <cstdint>
#include "storage.hpp"
#include "serialize.hpp"
#include "serializer.hpp"
#include "keyvaluestorage.hpp"
#include "rankdocserializer.hpp"
#include "docrankserializer.hpp"

int main(int argc, char** argv)
{
	if (argc < 3) { 
		std::cout << "Too few arguments\n";
		return 1;
	}

	if (std::string(argv[1]) == "forward") {
		std::cout << "Forward storage: \n\n";

		auto forward_storage = makeKeyValueStorage<double, std::uint64_t>(argv[2], RankDocSerializer());
		forward_storage.dumpStorage();
	} else { 
		std::cout << "Inverted storage: \n\n";

		auto inverted_storage = makeKeyValueStorage<std::uint64_t, double>(argv[2], DocRankSerializer());
		inverted_storage.dumpStorage();
	}
}

