#include <iostream>
#include <cstdint>
#include "storage.hpp"
#include "serialize.hpp"
#include "serializer.hpp"
#include "keyvaluestorage.hpp"

int main()
{
	struct uint32Serializer : public Serializer<std::uint32_t>
	{
		using Serializer<ValueType>::serialize;
		using Serializer<ValueType>::deserialize;

		std::size_t getValueSize() const override { return sizeof(std::uint32_t); }

		void serialize(std::uint32_t n, std::vector<char>& buf) override
		{
			buf.resize(sizeof(n));
			const char* b = reinterpret_cast<const char*>(&n);
			std::copy(b, b + sizeof(n), buf.data());
		}

		std::uint32_t deserialize(const char* buf) override
		{
			return *reinterpret_cast<const std::uint32_t*>(buf);
		}
	};

	struct pairSerializer : public Serializer<std::pair<std::uint32_t, std::uint32_t>>
	{
		using Serializer<ValueType>::serialize;
		using Serializer<ValueType>::deserialize;

		std::size_t getValueSize() const override { return 2 * sizeof(std::uint32_t); }

		void serialize(ValueType n, std::vector<char>& buf) override
		{
			buf.clear();

			std::vector<char> tmp;
			
			serializer.serialize(n.first, tmp);
			buf.insert(buf.end(), tmp.begin(), tmp.end());

			tmp.clear();
			
			serializer.serialize(n.second, tmp);
			buf.insert(buf.end(), tmp.begin(), tmp.end());
		}

		ValueType deserialize(const char* buf) override
		{
			return {serializer.deserialize(buf), serializer.deserialize(buf + sizeof(std::uint32_t))};
		}
		
		uint32Serializer serializer;
	};

//	auto storage = make_storage<std::pair<std::uint32_t, std::uint32_t>>("forward_storage.bin", pairSerializer());

	auto storage = makeKeyValueStorage<std::uint32_t, std::uint32_t>("forward_storage.bin", pairSerializer());

	int x;
	std::cin >> x;

	storage.add(x, x);

	for (auto it = storage.getIterator(); it.hasNext(); ++it) {
		std::cout << it->first << ' ' << it->second << '\n';
	}

	storage.dumpStorage();
}

