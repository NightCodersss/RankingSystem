#include "bigstorage.hpp"
#include "keyvaluestorage.hpp"
#include "rankdocserializer.hpp"
#include "docrankserializer.hpp"
#include <regex>

BigStorage::BigStorage(std::string path) : path(path) { }

void BigStorage::addCommit(Commit commit)
{
	if (commit.empty()) {
		return;
	}

	std::pair<std::string, TextID> current_file = {commit[0].word, commit[0].text_id};

	auto forward_serializer = RankDocSerializer();
	auto inverted_serializer = DocRankSerializer();

	auto filename = getFilename(current_file.first, current_file.second);
	auto forward_storage = makeKeyValueStorage<Rank, DocID>(filename + "_forward.bin", forward_serializer);
	auto inverted_storage = makeKeyValueStorage<DocID, Rank>(filename + "_inverted.bin", inverted_serializer);

	for (const auto& record: commit) {
		if (current_file != std::make_pair(record.word, record.text_id)) {
			current_file = {record.word, record.text_id};

			filename = getFilename(record.word, record.text_id);
			forward_storage  = makeKeyValueStorage<Rank, DocID>(filename + "_forward.bin", forward_serializer);
			inverted_storage = makeKeyValueStorage<DocID, Rank>(filename + "_inverted.bin", inverted_serializer);
		}
		
		auto doc_rank = inverted_storage.get(record.doc_id);
		if (doc_rank) {
			forward_storage.remove(*doc_rank, record.doc_id);
			inverted_storage.remove(record.doc_id, *doc_rank);
		}
		forward_storage.add(record.rank, record.doc_id);
		inverted_storage.add(record.doc_id, record.rank);
	}
}

std::string BigStorage::getFilename(std::string word, TextID text_id)
{
	std::regex r(R"([/\\])");
	word = std::regex_replace(word, r, "_");
	text_id = std::regex_replace(text_id, r, "_");

	return path + "/" + word + "_" + text_id;
}
