#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "keyvaluestorage.hpp"
#include "rankdocserializer.hpp"
#include "docrankserializer.hpp"
	
using TextID = std::string;
using Rank = double;
using DocID = std::uint64_t;

struct Record
{
	std::string word;
	TextID text_id;
	DocID doc_id;
	Rank rank;
};

using Commit = std::vector<Record>;

class BigStorage
{
public:
	BigStorage(std::string path = ".", std::size_t subdirectories_number=1000);
	void addCommit(Commit commit);

	std::unique_ptr<KeyValueStorage<Rank, DocID, RankDocSerializer>> getForwardStorage(std::string word, TextID text_id);
	std::unique_ptr<KeyValueStorage<DocID, Rank, DocRankSerializer>> getInvertedStorage(std::string word, TextID text_id);

	std::string getFilename(std::string word, TextID text_id);
private:

	std::string path;
	std::size_t subdirectories_number = 1000;
};
