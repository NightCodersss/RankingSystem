#pragma once

#include <queue>
#include "sender.hpp"

class RootSender : public SenderInterface
{
public:
	RootSender(int number_of_docs);

	void send(SocketStream& stream, ubjson::Value val) override;

private:
	int number_of_docs;
	std::queue<ubjson::Value> queue;
};
