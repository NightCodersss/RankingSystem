#include "block.hpp"

Block::Block(offset_t header_offset, offset_t data_offset, offset_t next_block_offset)
	: header_offset(header_offset)
	, data_offset(data_offset)
	, next_block_offset(next_block_offset)	
{
}

