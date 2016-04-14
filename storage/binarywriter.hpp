#pragma once

#include <string>
#include <fstream>
#include <cstdint>

class BinaryWriter
{
public:
	BinaryWriter(const std::string& filename);

	void writeUInt32(std::uint32_t n);
	
	void seekTo(int offset);	
	void seekBy(int offset);
	void seekFromEnd(int offset);

	int offset();

private:
	std::ofstream out;
};
