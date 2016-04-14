#include "blockreader.hpp"
#include "storage.hpp"

BlockReader::BlockReader(Storage& storage, int init_offset) 
	: storage(storage)
	, reader(storage.getFileName())
{
	reader.seekTo(init_offset);
}

boost::optional<DocID> BlockReader::readDocID()
{
	if (position_in_block == storage.blockSize() - 1) {
		int pos = reader.readUInt32();

		if (pos == 0) {
			return boost::none;
		}

		reader.seekTo(pos);
		position_in_block = 0;
		return readDocID();
	} else {
		++position_in_block;
		return reader.readUInt32();
	}
}
