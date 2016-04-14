#pragma once

#include <string>
#include <fstream>
#include <cstdint>

class BinaryReader
{
public:
	BinaryReader(const std::string& filename);
	bool isValid() const;

	std::uint32_t readUInt32();

	void seekTo(int offset);	
	void seekBy(int offset);
	void seekFromEnd(int offset);

	int offset();

private:
	std::ifstream in;
};
