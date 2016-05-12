#pragma once

#include <cstdint>
#include <vector>
#include <string>
	
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

private:
	std::string getFilename(std::string word, TextID text_id);

	std::string path;
	std::size_t subdirectories_number = 1000;
};
