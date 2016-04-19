#pragma once

#include <map>
#include <string>
#include <fstream>
#include <defines.hpp>
#include "serialize.hpp"
#include "block.hpp"
#include "storage_defines.hpp"
#include "storageiterator.hpp"
#include "blockwriter.hpp"

template <typename Value>
class Storage
{
    friend class StorageIterator<Storage>;
	friend class BlockWriter<Storage>;

	static const int default_block_capacity = 3;
public:
    using value_type = Value;

	Storage(const std::string& filename, std::size_t header_size, std::size_t value_size, std::size_t end_size) 
		: filename(filename)
		, header_size(header_size)
		, value_size(value_size)
		, end_size(end_size)
	{
        file_header_size = sizeof(std::size_t) * 2;

		std::ifstream in(filename, std::ios::binary);
		if (!in)
		{
			number_of_blocks = 0;
			block_capacity = default_block_capacity;
            
            Block block = newBlock();

			std::ofstream out(filename, std::ios::binary);
			serialize(number_of_blocks, out);
			serialize(block_capacity, out);

            out.seekp(block.endOffset());
            serialize((offset_t)0, out);
//			serialize(value_size, io);
		} 
		else 
		{
			number_of_blocks = deserialize<std::size_t>(in, sizeof(number_of_blocks));
			block_capacity = deserialize<std::size_t>(in, sizeof(block_capacity));
		}
	}

    ~Storage()
    {
        ReadWriteFileStream out(filename);
        serialize(number_of_blocks, out);
        serialize(block_capacity, out);
    }

	Value get(std::size_t block, std::size_t pos_in_block)
	{
		check(block, pos_in_block);

		std::ifstream in(filename);
		in.seekg(getBlockInfo(block).getOffset(pos_in_block));

		return deserialize<Value>(in, value_size);
	}

	void add(std::size_t block, Value value)
	{
		check(block, 0);

		BlockWriter<Storage> writer(*this);
		writer.write(value, getBlockInfo(block));
	}

	StorageIterator<Storage> getIterator() const
	{
        if (number_of_blocks <= 0) {
            throw std::logic_error("empty storage");
        }
		return StorageIterator<Storage>(*this, getBlockInfo(0));
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
			throw std::invalid_argument("Attempt to use non-existent block; All size in blocks: " + std::to_string(number_of_blocks) + " Block: " + std::to_string(block) + ", Pos in block: " + std::to_string(pos_in_block));
		}
		return true;
	}

	offset_t getOffset(std::size_t block) const { return file_header_size + block * (header_size + value_size * block_capacity + end_size); } 
	Block newBlock() { return getBlockInfo(number_of_blocks++); }

	Block getBlockInfo(std::size_t block) const
	{
		auto off = getOffset(block);
		return Block(off, header_size, block_capacity, value_size);
	}

	std::size_t number_of_blocks;
	std::size_t block_capacity;

    std::size_t file_header_size;

	std::size_t header_size;
	std::size_t value_size;
	std::size_t end_size;
};
