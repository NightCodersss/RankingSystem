#include "root_sender.hpp"

RootSender::RootSender(int number_of_docs) : number_of_docs(number_of_docs)
{
}

void RootSender::send(SocketStream& stream, ubjson::Value val)
{
	queue.push(val);

	if (queue.size() >= number_of_docs) {
		ubjson::StreamWriter<SocketStream> writer(stream);

		ubjson::Value batch;

		for (int i = 0; i < number_of_docs; ++i) {
			batch["docs"].push_back(queue.front());
			queue.pop();
		}

		writer.writeValue(batch);
	}
}
