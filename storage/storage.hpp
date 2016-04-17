#pragma once

#include <map>
#include <string>
#include <fstream>
#include <defines.hpp>
#include "binaryreader.hpp"
#include "binarywriter.hpp"
#include "serialize.hpp"
#include "block.hpp"

template <typename Value>
class Storage
{
	friend class BlockWriter;

	static const int default_block_capacity = 100;
	using offset_t = std::size_t;
public:
	Storage(const std::string& filename, std::size_t header_size, std::size_t value_size, std::size_t end_size) 
		: filename(filename)
		, header_size(header_size)
		, value_size(value_size)
		, end_size(end_size)
	{
		std::ifstream in(filename, std::ios::binary);
		if (!in)
		{
			number_of_blocks = 0;
			block_capacity = default_block_capacity;

			std::ofstream out(filename, std::ios::binary);
			serialize(number_of_blocks, io);
			serialize(block_capacity, io);
//			serialize(value_size, io);
		} 
		else 
		{
			number_of_blocks = deserialize<std::size_t>(in, sizeof(number_of_blocks));
			block_capacity = deserialize<std::size_t>(in, sizeof(block_capacity));
		}
	}

	Value get(std::size_t block, std::size_t pos_in_block)
	{
		check(block, pos_in_block);

		std::ifstream in(filename);
		in.seekg(getBlockInfo(block).getOffset(pos_in_block));

		return deserialize<Value>(in, value_size);
	}

	void add(std::size_t block, std::size_t pos_in_block, Value value)
	{
		check(block, pos_in_block);

		BlockWriter writer(*this, getBlockInfo(block));
		writer.write(value);
	}

	StorageIterator getIterator() const
	{
		return StorageIterator(0);
	}

	std::string getFileName() const { return filename; }
	int blockCapacity() const { return block_capacity; }

private:
	std::string filename;

	bool checkNoThrow(std::size_t block, std::size_t pos_in_block) 
	{
		return 0 <= block && block < number_of_blocks 
		    && 0 <= pos_in_block && pos_in_block < block_capacity;
	}

	bool check(std::size_t block, std::size_t pos_in_block)
	{
		if (!checkNoThrow(block, pos_in_block)) {
			throw std::invalid_argument("Attempt to use non-existent block; Block: " + std::to_string(block) + ", Pos in block: " + std::to_string(pos_in_block));
		}
		return true;
	}

	offset_t getOffset(std::size_t block) const { return block * (header_size + value_size + end_size); } 
	Block newBlock() { return getBlockInfo(number_of_blocks++); }

	Block getBlockInfo(std::size_t block) const
	{
		auto off = getOffset(block);
		return Block(off, off + header_size, off + header_size + block_capacity * value_size, value_size);
	}

	std::size_t number_of_blocks;
	std::size_t block_capacity;

	std::size_t header_size;
	std::size_t value_size;
	std::size_t end_size;
};
