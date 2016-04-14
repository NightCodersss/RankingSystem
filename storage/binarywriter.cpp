#include "binarywriter.hpp"

BinaryWriter::BinaryWriter(const std::string& filename) : out(filename, std::ios::binary)
{

}

void BinaryWriter::writeUInt32(std::uint32_t n)
{
	out.write(reinterpret_cast<char*>(&n), sizeof(n));	
}

void BinaryWriter::seekTo(int offset)
{
	out.seekp(offset, std::ios_base::beg);
}

void BinaryWriter::seekBy(int offset)
{
	out.seekp(offset, std::ios_base::cur);
}

void BinaryWriter::seekFromEnd(int offset)
{
	out.seekp(offset, std::ios_base::end);
}

int BinaryWriter::offset()
{
	return out.tellp();
}
