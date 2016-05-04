#pragma once

#include <utility>
#include <cstdint>
#include "serializer.hpp"

using Rank = double;
using DocID = std::uint64_t;

struct DocRankSerializer : public Serializer<std::pair<DocID, Rank>>
{
	using Serializer<ValueType>::serialize;
	using Serializer<ValueType>::deserialize;

	std::size_t getValueSize() const override { return sizeof(std::uint64_t) + sizeof(double); }

	void serialize(ValueType n, std::vector<char>& buf) override
	{
		buf.clear();
		buf.resize(0);

		std::vector<char> tmp;

		tmp.resize(sizeof(n.first));
		const char* b = reinterpret_cast<const char*>(&n.first);
		std::copy(b, b + sizeof(n.first), tmp.data());
		buf.insert(buf.end(), tmp.begin(), tmp.end());

		tmp.clear();
		tmp.resize(0);

		tmp.resize(sizeof(n.second));
		b = reinterpret_cast<const char*>(&n.second);
		std::copy(b, b + sizeof(n.second), tmp.data());
		buf.insert(buf.end(), tmp.begin(), tmp.end());
	}

	ValueType deserialize(const char* buf) override
	{
		return {*reinterpret_cast<const std::uint64_t*>(buf), *reinterpret_cast<const double*>(buf + sizeof(std::uint64_t))};
	}
};
