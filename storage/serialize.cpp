#include "serialize.hpp"

template <>
void serialize<std::uint32_t>(std::uint32_t n, std::vector<char>& buf)
{
	buf.resize(sizeof(n));
	const char* b = reinterpret_cast<const char*>(&n);
	std::copy(b, b + sizeof(n), buf.data());
}

template <>
std::uint32_t deserialize<std::uint32_t>(const char* buf)
{
	return *reinterpret_cast<const std::uint32_t*>(buf);
}

template <>
void serialize<std::size_t>(std::size_t n, std::vector<char>& buf)
{
	buf.resize(sizeof(n));
	const char* b = reinterpret_cast<const char*>(&n);
	std::copy(b, b + sizeof(n), buf.data());
}

template <>
std::size_t deserialize<std::size_t>(const char* buf)
{
	return *reinterpret_cast<const std::size_t*>(buf);
}
