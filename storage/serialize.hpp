#pragma once
#include <vector>
#include <iostream>

template <typename T>
void serialize(T t, std::vector<char>& buf);

template <typename T>
std::vector<char> serialize(T t)
{
	std::vector<char> buf;
	serialize(t, buf);
	return buf;
}

template <typename T>
void serialize(T t, std::ostream& out)
{
	auto buf = serialize(t);
	out.write(buf.data(), buf.size());
}

template <typename T>
T deserialize(const char* buf);

template <typename T>
T deserialize(std::istream& in, std::size_t value_size)
{
	std::vector<char> buf(value_size);
	in.read(buf.data(), value_size);
	return deserialize<T>(buf.data());
}

template <typename T>
std::vector<T> deserialize(const char* buf, std::size_t size, std::size_t value_size)
{
	std::vector<T> values;
	for (std::uint32_t i = 0; i < size; ++i) {
		values.emplace_back(deserialize<T>(buf + i * value_size));
	}
	return std::move(values);
}
