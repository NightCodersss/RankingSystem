#pragma once

#include <defines.hpp>

class Storage;

template <typename Value>
class BlockWriter
{
public:
	BlockWriter(Storage<Value>& storage) : storage(storage) { }
	
	using offset_t = typename Storage<Value>::offset_t;

	void write(Value value, offset_t block_offset, offset_t in_block_offset)
	{
		std::fstream io(filename, std::ios::binary);

		io.seekg(block_offset, io.beg);
		std::uint32_t block_size = deserialize(io, sizeof(block_size));

		std::vector<char> buf(storage.value_size * block_size);
		io.read(buf.data(), buf.size());

		std::vector<Value> values = deserialize<Value>(buf, block_size, storage.values_size);
		values.emplace_back(value);
		std::sort(values.begin(), values.end());

		if (block_size == storage.blockCapacity()) {		
			io.seekp(block_offset, io.beg);
			serialize(values.size() / 2, io);

			for (auto it = values.rbegin(); it != values.rbegin() + values.size() / 2; ++it) {
				serialize(*it, io);
			}

			offset_t block_end = block_offset + storage.blockCapacity() * storage.value_size + sizeof(offset_t);
			io.seekg(block_end, io.beg);	
			offset_t next_block_offset = deserialize(io, sizeof(next_block_offset));

			offset_t new_block_offset = storage.newBlock();
			io.seekp(block_end, io.beg);
			serialize(new_block_offset, io);

			io.seekp(new_block_offset, io.beg);
			serialize(values.size() - values.size() / 2, io);
			for (auto it = values.rbegin() + values.size() / 2; it != values.rend(); ++it) {
				serialize(*it, io);
			}

			offset_t next_block_end = new_block_offset + storage.blockCapacity() * storage.value_size + sizeof(offset_t);
			io.seekp(next_block_end, io.beg);
			serialize(next_block_offset, io);

		} else {
			io.seekp(block_offset);
			serialize(block_size + 1, io);

			for (auto it = values.rbegin(); it != values.rend(); ++it) {
				serialize(*it, io);
			}
		}
	}

private:
	Storage<Value>& storage;
};
