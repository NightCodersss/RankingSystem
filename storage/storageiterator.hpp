#pragma once

#include <fstream>
#include "block.hpp"

template <typename Storage>
class StorageIterator
{
public:
    using Value = typename Storage::value_type;
    StorageIterator(const Storage& storage, Block start_block) 
        : storage(storage)
        , start_block(start_block)
        , pos_in_block(0)
        , is_end(false)
        , stream(storage.filename, std::ios::binary)
    {
        stream.seekg(start_block.headerOffset());
        stream.read(reinterpret_cast<char*>(&block_size), sizeof(block_size));

        if (block_size == 0) {
            is_end = true;
            return;
        }

        stream.seekg(start_block.dataOffset());
        last_value = deserialize<Value>(stream, storage.value_size);
    }
    
    Value peek() { return last_value; }

    void next() 
    { 
        if (is_end) {
            throw std::range_error("Storage iterator has finished");
        }

        ++pos_in_block;
        if (pos_in_block == block_size) {
            stream.seekg(start_block.endOffset());
            
            offset_t next_offset;
            stream.read(reinterpret_cast<char*>(&next_offset), sizeof(next_offset));

            if (next_offset == 0) {
                is_end = true;
                return;
            }

            start_block = Block(next_offset, storage.header_size, storage.block_capacity, storage.value_size);
            
            stream.seekg(start_block.headerOffset());
            stream.read(reinterpret_cast<char*>(&block_size), sizeof(block_size));
            stream.seekg(start_block.dataOffset());
            pos_in_block = 0;
        } 
        last_value = deserialize<Value>(stream, storage.value_size);
    }

    bool hasNext() const { return !is_end; }

    Value operator*() { return peek(); }
    StorageIterator& operator++() { next(); return *this; }

private:
    const Storage& storage;
    Block start_block;

    int pos_in_block;
    std::size_t block_size;

    Value last_value;

    bool is_end;
    std::ifstream stream;
};
