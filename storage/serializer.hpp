#pragma once
#include <iostream>

template <typename T>
class Serializer
{
public:
    virtual void serialize(T t, std::vector<char>& buf) = 0;
    virtual T deserialize(const char* buf) = 0;

    virtual std::size_t getValueSize() const = 0;

    std::vector<char> serialize(T t)
    {
        std::vector<char> buf;
        serialize(t, buf);
        return buf;
    }

    template <typename Stream = std::ostream>
    void serialize(T t, Stream& out)
    {
        auto buf = serialize(t);
        out.write(buf.data(), buf.size());
    }

    template <typename Stream = std::istream>
    T deserialize(Stream& in)
    {
        std::vector<char> buf(getValueSize());
        in.read(buf.data(), getValueSize());
        return deserialize(buf.data());
    }

    std::vector<T> deserialize(const char* buf, std::size_t size)
    {
        std::vector<T> values;
        for (std::uint32_t i = 0; i < size; ++i) {
            values.emplace_back(deserialize(buf + i * getValueSize()));
        }
        return std::move(values);
    }
};


