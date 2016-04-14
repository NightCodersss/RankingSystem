#pragma once

#include <boost/optional.hpp>
#include <defines.hpp>
#include "binaryreader.hpp"

class Storage;

class BlockReader
{
public:
	BlockReader(Storage& storage, int init_offset);
	boost::optional<DocID> readDocID();

private:
	BinaryReader reader;
	int position_in_block = 0;
	Storage& storage;
};
