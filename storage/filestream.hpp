#pragma once

#include <cstdio>
#include <string>
#include "storage_defines.hpp"

class ReadWriteFileStream
{
public:
    ReadWriteFileStream();
    ReadWriteFileStream(std::string filename);

    ~ReadWriteFileStream();

    void read(char* buf, std::size_t size);
    void write(const char* buf, std::size_t size);

    void seekg(offset_t offset);
    void seekp(offset_t offset);

private:
    std::FILE* file;

    offset_t read_pos = 0;
    offset_t write_pos = 0;
};
