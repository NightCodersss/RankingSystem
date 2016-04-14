#include "binaryreader.hpp"

BinaryReader::BinaryReader(const std::string& filename) : in(filename, std::ios::binary)
{

}

std::uint32_t BinaryReader::readUInt32()
{
	std::uint32_t n;
	in.read(reinterpret_cast<char*>(&n), sizeof(n));

	return n;
}

bool BinaryReader::isValid() const { return (bool)in; }
	
void BinaryReader::seekTo(int offset)
{
	in.seekg(offset, std::ios_base::beg);
}

void BinaryReader::seekBy(int offset)
{
	in.seekg(offset, std::ios_base::cur);
}

void BinaryReader::seekFromEnd(int offset)
{
	in.seekg(offset, std::ios_base::end);
}

int BinaryReader::offset()
{
	return in.tellg();
}
