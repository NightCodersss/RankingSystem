#pragma once

#include "storage_defines.hpp"

class Block
{
public:
	Block(offset_t header_offset, std::size_t header_size, std::size_t block_capacity, std::size_t value_size);

	offset_t getOffset(std::size_t pos_in_block) const;

    offset_t headerOffset() const;
    offset_t dataOffset() const;
    offset_t endOffset() const;

private:
	offset_t header_offset;
    
    std::size_t header_size;
    std::size_t block_capacity;
    std::size_t value_size;
};
