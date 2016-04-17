#pragma once

class Block
{
public:
	Block(offset_t header_offset, offset_t data_offset, offset_t end_offset, std::size_t value_size);

	offset_t getOffset(std::size_t pos_in_block);

	offset_t header_offset;
	offset_t data_offset;
	offset_t end_offset;
	std::size_t value_size;
};
