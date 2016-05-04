#include "filestream.hpp"

ReadWriteFileStream::ReadWriteFileStream()
    : file(nullptr)
{
}

ReadWriteFileStream::ReadWriteFileStream(const std::string& filename)
{
    file = std::fopen(filename.data(), "r+b");
	if (!file) {
		file = std::fopen(filename.data(), "w+b");
	}

	std::fseek(file, 0, SEEK_SET);
}

ReadWriteFileStream::~ReadWriteFileStream()
{
    if (file != nullptr) {
        std::fclose(file);
    }
}

void ReadWriteFileStream::read(char* buf, std::size_t size)
{
    std::fseek(file, read_pos, SEEK_SET);
    std::fread(buf, sizeof(char), size, file);
    read_pos = std::ftell(file);
}

void ReadWriteFileStream::write(const char* buf, std::size_t size)
{
    std::fseek(file, write_pos, SEEK_SET);
    std::fwrite(buf, sizeof(char), size, file);
    write_pos = std::ftell(file);
}
    
void ReadWriteFileStream::seekg(offset_t offset)
{
    read_pos = offset;
}

void ReadWriteFileStream::seekp(offset_t offset)
{
    write_pos = offset;
}
