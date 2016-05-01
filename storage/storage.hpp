#pragma once

#include <map>
#include <string>
#include <set>
#include <fstream>
#include <boost/optional.hpp>
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

	static const int default_block_capacity = 3;
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

	Value get(std::size_t block_number, std::size_t pos_in_block)
	{
		check(block_number, pos_in_block);

		std::ifstream in(filename);
		in.seekg(getBlockInfo(block_number).getOffset(pos_in_block));

		return serializer.deserialize(in);
	}

	void add(std::size_t block, Value value)
	{
		check(block, 0);

		BlockWriter<Storage> writer(*this);
		writer.write(value, getBlockInfo(block));

		resortTable();
	}

	StorageIterator<Storage> getIterator(std::size_t block_number = 0) 
	{
        if (number_of_blocks <= 0) {
            throw std::logic_error("empty storage");
        }
		return StorageIterator<Storage>(*this, getBlockInfo(block_number));
	}

	std::string getFileName() const { return filename; }
	int blockCapacity() const { return block_capacity; }
	auto numberOfBlocks() const { return number_of_blocks; }

	boost::optional<std::size_t> findBlockContaining(Value value)
	{
		ReadWriteFileStream io(table_filename);
		for (std::size_t i = 0; i < number_of_blocks; ++i) {
			auto max = serializer.deserialize(io);
			auto min = serializer.deserialize(io);

			if (min <= value && value <= max) {
				return i;
			}
		}

		return boost::none;
	}

	boost::optional<std::size_t> findPosInBlock(std::size_t block_number, Value value)
	{
		auto block = getBlockInfo(block_number);
		ReadWriteFileStream io(filename);

		io.seekg(block.headerOffset());
		auto block_size = deserialize<std::size_t>(io, sizeof(std::size_t));

		io.seekg(block.dataOffset());

		for (std::size_t i = 0; i < block_size; ++i) {
			auto val = serializer.deserialize(io);
			if (val == value) {
				return i;
			}
		}

		return boost::none;
	}

	void removeFromBlock(std::size_t block_number, std::size_t pos_in_block)
	{
        ReadWriteFileStream io(filename); 
		
		auto block = getBlockInfo(block_number);

		io.seekg(block.headerOffset());
        std::size_t block_size = deserialize<std::size_t>(io, sizeof(block_size));

		// Get values in block
		std::vector<char> buf(value_size * block_size);
		io.seekg(block.dataOffset());
		io.read(buf.data(), buf.size());
		
		std::vector<Value> values = serializer.deserialize(buf.data(), block_size);
		values.erase(values.begin() + pos_in_block);
		std::sort(values.begin(), values.end(), std::greater<Value>());

		auto max_val = values.front();
		auto min_val = values.back();

		io.seekp(block.headerOffset());
		serialize(block_size - 1, io);
		serializer.serialize(max_val, io);
		serializer.serialize(min_val, io);

		io.seekp(block.dataOffset());
		for (auto it: values) { serializer.serialize(it, io); }
		
		tableUpdate(block.blockNumber(), max_val, min_val);
		resortTable();
	}

	Block getBlockInfo(std::size_t block) const
	{
		auto off = getOffset(block);
		return Block(block, off, header_size, block_capacity, value_size);
	}

	void dumpStorage()
	{
		std::cout << "Storage:\n";
		for (auto it = getIterator(); it.hasNext(); ++it) {
			std::cout << "Block: " << it.start_block.blockNumber() << "; ";
			std::cout << "size: " << it.block_size << "; ";
			std::cout << "value: " << "(" << it->first << ' ' << it->second << ")" << "; ";
			std::cout << "\n";
		}
		
		ReadWriteFileStream io(table_filename);

		std::vector<char> buf((value_size * 2 + sizeof(std::size_t)) * (number_of_blocks));
		io.read(buf.data(), buf.size());

		std::cout << "\n\nTable:\n";
		for (int i = 0; i < number_of_blocks; ++i) {
			auto max = serializer.deserialize(buf.data() + i * (2 * value_size + sizeof(std::size_t)));
			auto min = serializer.deserialize(buf.data() + i * (2 * value_size + sizeof(std::size_t)) + value_size);
			auto block_num = deserialize<std::size_t>(buf.data() + i * (2 * value_size + sizeof(std::size_t)) + value_size * 2);
			
			std::cout << "max: " << "(" << max.first << ' ' << max.second << ")" << "; ";
			std::cout << "min: " << "(" << min.first << ' ' << min.second << ")" << "; ";
			std::cout << "block_number: " << block_num << "; ";
			std::cout << "\n";
		}
	}

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
		auto block_number = number_of_blocks++;

		new_blocks.insert(block_number);
        return getBlockInfo(block_number);
    }

	void tableUpdate(std::size_t block, Value max, Value min)
	{
		table_updates[block] = {max, min};
	}

	void resortTable()
	{
		ReadWriteFileStream io(table_filename);

		std::vector<char> buf((value_size * 2 + sizeof(std::size_t)) * (number_of_blocks - new_blocks.size()));
		io.read(buf.data(), buf.size());

		std::vector<std::tuple<Value, Value, std::size_t>> values;
		for (int i = 0; i < number_of_blocks - new_blocks.size(); ++i) {
			auto max = serializer.deserialize(buf.data() + i * (2 * value_size + sizeof(std::size_t)));
			auto min = serializer.deserialize(buf.data() + i * (2 * value_size + sizeof(std::size_t)) + value_size);
			auto block_num = deserialize<std::size_t>(buf.data() + i * (2 * value_size + sizeof(std::size_t)) + value_size * 2);

			auto it = table_updates.find(block_num);
			if (it == table_updates.end()) {
				values.emplace_back(max, min, block_num);
			} else {
				auto max = it->second.first;
				auto min = it->second.second;
				values.emplace_back(max, min, block_num);
			}
		}

		for (auto block_number: new_blocks) {
			auto it = table_updates.find(block_number);
			if (it != table_updates.end()) {
				auto max = it->second.first;
				auto min = it->second.second;
				values.emplace_back(max, min, block_number);
			} else {
				values.emplace_back(Value{}, Value{}, block_number);
			}
		}
		new_blocks.clear();

		std::sort(values.begin(), values.end(), std::greater<std::tuple<Value, Value, std::size_t>>());
		io.seekp(0);
		for (const auto& it: values) { 
			serializer.serialize(std::get<0>(it), io);
			serializer.serialize(std::get<1>(it), io);
			serialize(std::get<2>(it), io);
		}

		table_updates.clear();
	}

	std::string filename;
    std::string table_filename;

	std::size_t number_of_blocks;
	std::size_t block_capacity;

    std::size_t file_header_size;

	std::size_t header_size;
	std::size_t value_size;
	std::size_t end_size;

	std::map<std::size_t, std::pair<Value, Value>> table_updates;
	std::set<std::size_t> new_blocks;

    Serializer serializer;
};

template <typename Value, typename Serializer>
auto make_storage(const std::string& filename, Serializer serializer)
{
    return Storage<Value, Serializer>(filename, serializer);
}
