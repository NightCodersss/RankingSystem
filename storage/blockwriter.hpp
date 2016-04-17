#pragma once

#include <defines.hpp>
#include "block.hpp"

class Storage;

template <typename Value>
class BlockWriter
{
public:
	BlockWriter(Storage<Value>& storage) : storage(storage) { }
	
	using offset_t = typename Storage<Value>::offset_t;

	void write(Value value, Block block)
	{
		std::fstream io(storage.filename, std::ios::binary);

		io.seekg(block.header_offset, io.beg);
		auto block_size = deserialize<std::uint32_t>(io, sizeof(block_size));

		// Get values in block
		std::vector<char> buf(block.value_size * block_size);
		io.seekg(block.data_offset, io.beg);
		io.read(buf.data(), buf.size());
		
		std::vector<Value> values = deserialize<Value>(buf, block_size, block.values_size);
		values.emplace_back(value);
		std::sort(values.begin(), values.end(), std::greater<Value>());

		auto max_val = values.front();
		auto min_val = values.back();

		if (block_size == storage.blockCapacity()) {		
			Block new_block = storage.newBlock();

			std::size_t first_block_size = values.size() / 2;
			std::size_t second_block_size = values.size() - first_block_size;

			auto first_min = values[first_block_size - 1];
			auto second_max = values[first_block_size];

			io.seekp(block.header_offset, io.beg);
			serialize(first_block_size, io);
			serialize(max_val, io);
			serialize(first_min, io);

			io.seekp(block.data_offset, io.beg);
			for (auto it = values.begin(); it != values.begin() + first_block_size; ++it) {
				serialize(*it, io);
			}

			io.seekg(block.end_offset, io.beg);	
			offset_t next_block_offset = deserialize<offset_t>(io, sizeof(next_block_offset));

			io.seekp(block.end_offset, io.beg);
			serialize(new_block.header_offset, io);

			io.seekp(new_block.header_offset, io.beg);
			serialize(second_block_size, io);
			serialize(second_max, io);
			serialize(min_val, io);

			io.seekp(new_block.data_offset, io.beg);
			for (auto it = values.begin() + first_block_size; it != values.end(); ++it) {
				serialize(*it, io);
			}

			io.seekp(new_block.end_offset, io.beg);
			serialize(next_block_offset, io);
			
		} else {
			io.seekp(block.header_offset, io.beg);
			serialize(block_size + 1, io);
			serialize(max_val, io);
			serialize(min_val, io);

			io.seekp(block.data_offset, io.beg);
			for (auto it = values.begin(); it != values.end(); ++it) {
				serialize(*it, io);
			}
		}
	}

private:
	Storage<Value>& storage;
};
