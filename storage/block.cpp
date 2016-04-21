#include "block.hpp"

Block::Block(std::size_t block_number
           , offset_t header_offset
           , std::size_t header_size
           , std::size_t block_capacity
           , std::size_t value_size)
    : block_number(block_number)
	, header_offset(header_offset)
	, header_size(header_size)
	, block_capacity(block_capacity)
    , value_size(value_size)
{
}

offset_t Block::getOffset(std::size_t pos_in_block) const { return dataOffset() + pos_in_block * value_size; }

offset_t Block::headerOffset() const { return header_offset; }
offset_t Block::dataOffset() const { return header_offset + header_size; }
offset_t Block::endOffset() const { return header_offset + header_size + block_capacity * value_size; }

std::size_t Block::blockNumber() const { return block_number; }
