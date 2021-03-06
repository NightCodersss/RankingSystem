#pragma once

#include <memory>
#include "storage.hpp"

template <typename Key, typename Value, typename Serializer>
class KeyValueStorage
{
public:
	KeyValueStorage(const std::string& filename, Serializer serializer) 
		: storage(filename, serializer)
		, serializer(serializer)
	{
		std::ifstream io(storage.getFileName() + "_table");
		
		if (io) {
			std::vector<char> buf((valueSize() * 2 + sizeof(std::size_t)) * storage.numberOfBlocks());
			io.read(buf.data(), buf.size());

			std::ifstream in(storage.getFileName(), std::ios::binary);
			blocks.clear();
			for (int i = 0; i < storage.numberOfBlocks(); ++i) {
				auto block_num = deserialize<std::size_t>(buf.data() + i * (2 * valueSize() + sizeof(std::size_t)) + valueSize() * 2);

				in.seekg(storage.getBlockInfo(block_num).headerOffset());
				auto block_size = deserialize<std::size_t>(in, sizeof(std::size_t));

				blocks.emplace_back(block_num, block_size);
			}
		}
	}

	auto valueSize() const { return serializer.getValueSize(); }

	void add(Key key, Value value)
	{
		auto block_number = findBlock(key);
		storage.add(block_number, {key, value});

		updateBlocksInfo();
	}

	boost::optional<Value> get(Key key)
	{
		auto block_number = findBlock(key);
		if (block_number >= blocks.size()) {
			return boost::none;
		}
		auto pos_in_block = storage.findPosInBlockLowerBound(block_number, {key, Value()});
		
		if (pos_in_block >= storage.blockCapacity()) {
			return boost::none;
		}

		auto key_value = storage.get(block_number, pos_in_block);

		if (key_value.first != key) {
			return boost::none;
		} else {
			return key_value.second;
		}
	}

	void remove(Key key) 
	{
		auto block_number = findBlock(key);
		auto pos_in_block = storage.findPosInBlockLowerBound(block_number, {key, Value()});
		storage.removeFromBlock(block_number, pos_in_block);
		
		updateBlocksInfo();
	}
	
	void remove(Key key, Value value) 
	{
		auto block_number = findBlock(key);
		auto pos_in_block = storage.findPosInBlock(block_number, {key, value});
		if (pos_in_block) {
			storage.removeFromBlock(block_number, *pos_in_block);
			updateBlocksInfo();
		}
	}

	std::size_t findBlock(Key key)
	{
		/*
		std::size_t current_sum = 0;
		std::vector<std::size_t> block_sums;
	
//		block_sums.push_back(0);
		for (int i = 0; i < blocks.size(); ++i) {
			current_sum += blocks[i].block_size;
			block_sums.push_back(current_sum);
		}
		*/

		int l = 0;
//		int r = block_sums.size();
		int r = blocks.size();

		while (r - l > 1) 
		{
			int m = (l + r) / 2;
			
			if (key > storage.get(blocks[m].block_number, 0).first) {
				r = m;
			} else {
				l = m;
			}
		}

		return l >= blocks.size() ? l : blocks[l].block_number;
	}

	auto getIterator() 
	{
		return storage.getIterator(blocks[0].block_number);
	}

	void dumpStorage()
	{
		storage.dumpStorage();
	}
	
private:
	void updateBlocksInfo()
	{
		std::ifstream io(storage.getFileName() + "_table");
		std::vector<char> buf((valueSize() * 2 + sizeof(std::size_t)) * storage.numberOfBlocks());
		io.read(buf.data(), buf.size());

		std::ifstream in(storage.getFileName(), std::ios::binary);
		blocks.clear();
		for (int i = 0; i < storage.numberOfBlocks(); ++i) {
			auto block_num = deserialize<std::size_t>(buf.data() + i * (2 * valueSize() + sizeof(std::size_t)) + valueSize() * 2);

			in.seekg(storage.getBlockInfo(block_num).headerOffset());
			auto block_size = deserialize<std::size_t>(in, sizeof(std::size_t));

			blocks.emplace_back(block_num, block_size);
		}
	}

	struct BlockInfo
	{
		BlockInfo(std::size_t block_number, std::size_t block_size) 
			: block_number(block_number)
			, block_size(block_size)
		{
		}

		std::size_t block_number;
		std::size_t block_size;
	};

	std::vector<BlockInfo> blocks;
	Storage<std::pair<Key, Value>, Serializer> storage;
	Serializer serializer;
};

template <typename Key, typename Value, typename Serializer>
auto makeKeyValueStorage(const std::string& filename, Serializer serializer)
{
	return std::make_unique<KeyValueStorage<Key, Value, Serializer>>(filename, serializer);
}
