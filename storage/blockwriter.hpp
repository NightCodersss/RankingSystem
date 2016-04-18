#pragma once

#include <defines.hpp>
#include "block.hpp"
#include "storage_defines.hpp"
#include "filestream.hpp"

template <typename Storage>
class BlockWriter
{
public:
    using Value = typename Storage::value_type;
	BlockWriter(Storage& storage) : storage(storage) { }
	
	void write(Value value, Block block)
	{
        ReadWriteFileStream io(storage.filename); // opens in binary mode

		io.seekg(block.headerOffset());
        std::size_t block_size = deserialize<std::size_t>(io, sizeof(block_size));

		// Get values in block
		std::vector<char> buf(storage.value_size * block_size);
		io.seekg(block.dataOffset());
		io.read(buf.data(), buf.size());
		
		std::vector<Value> values = deserialize<Value>(buf.data(), block_size, storage.value_size);
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

			io.seekp(block.headerOffset());
			serialize(first_block_size, io);
			serialize(max_val, io);
			serialize(first_min, io);

			io.seekp(block.dataOffset());
			for (auto it = values.begin(); it != values.begin() + first_block_size; ++it) {
				serialize(*it, io);
			}

			io.seekg(block.endOffset());
			offset_t next_block_offset = deserialize<offset_t>(io, sizeof(next_block_offset));

			io.seekp(block.endOffset());
			serialize(new_block.headerOffset(), io);

			io.seekp(new_block.headerOffset());
			serialize(second_block_size, io);
			serialize(second_max, io);
			serialize(min_val, io);

			io.seekp(new_block.dataOffset());
			for (auto it = values.begin() + first_block_size; it != values.end(); ++it) {
				serialize(*it, io);
			}

			io.seekp(new_block.endOffset());
			serialize(next_block_offset, io);
			
		} else {
			io.seekp(block.headerOffset());
			serialize(block_size + 1, io);
			serialize(max_val, io);
			serialize(min_val, io);
//            io.flush();

			io.seekp(block.dataOffset());
			for (auto it = values.begin(); it != values.end(); ++it) {
				serialize(*it, io);
			}
		}
	}

private:
	Storage& storage;
};
