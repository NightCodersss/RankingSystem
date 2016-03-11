#pragma once

#include <queue>
#include "sender.hpp"

class BatchSender : public SenderInterface
{
public:
	BatchSender(SocketStream& stream, int number_of_docs, int number_of_batches);
	~BatchSender();

	void send(ubjson::Value val) override;

public:
	int sent;

private:
	void formAndSend();
	
private:
	std::queue<ubjson::Value> queue;
	int number_of_docs;
	int number_of_batches;
};
