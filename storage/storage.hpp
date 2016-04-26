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

template <typename Value, typename Serializer>
class Storage
{
    friend class StorageIterator<Storage>;
	friend class BlockWriter<Storage>;

	static const int default_block_capacity = 10;
public:
    using value_type = Value;

	Storage(const std::string& filename, Serializer serializer)
		: filename(filename)
		, value_size(serializer.getValueSize())
        , serializer(serializer)
	{
        table_filename = filename + "_table";
        file_header_size = sizeof(std::size_t) * 2;

        header_size = sizeof(std::size_t) + 2 * value_size; // Number of filled cells in block and max/min
        end_size = sizeof(offset_t);

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

            std::ofstream table_out(table_filename, std::ios::binary);
            table_out.write("", 0);
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

		return serializer.deserialize(in);
	}

	void add(std::size_t block, Value value)
	{
		check(block, 0);

		BlockWriter<Storage> writer(*this);
		writer.write(value, getBlockInfo(block));

		resortTable();
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

    Block newBlock() 
    {
        return getBlockInfo(++number_of_blocks); 
    }

	Block getBlockInfo(std::size_t block) const
	{
		auto off = getOffset(block);
		return Block(block - 1, off, header_size, block_capacity, value_size);
	}

	void resortTable()
	{
		ReadWriteFileStream io(table_filename);

		std::vector<char> buf(value_size * 2 * number_of_blocks);
		io.read(buf.data(), buf.size());

		std::vector<std::pair<Value, Value>> values;
		for (int i = 0; i < number_of_blocks; ++i) {
			auto max = serializer.deserialize(buf.data() + i * 2 * value_size);
			auto min = serializer.deserialize(buf.data() + (i * 2 + 1) * value_size);

			values.emplace_back(max, min);
		}

		std::sort(values.begin(), values.end(), std::greater<std::pair<Value, Value>>());
		io.seekp(0);
		for (auto it = values.begin(); it != values.end(); ++it) { 
			serializer.serialize(it->first, io);
			serializer.serialize(it->second, io);
		}
	}

	std::string filename;
    std::string table_filename;

	std::size_t number_of_blocks;
	std::size_t block_capacity;

    std::size_t file_header_size;

	std::size_t header_size;
	std::size_t value_size;
	std::size_t end_size;

    Serializer serializer;
};

template <typename Value, typename Serializer>
auto make_storage(const std::string& filename, Serializer serializer)
{
    return Storage<Value, Serializer>(filename, serializer);
}
