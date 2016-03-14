#include "batch_sender.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

BatchSender::BatchSender(SocketStream& stream, int number_of_docs, int number_of_batches) 
	: SenderInterface(stream)
	, number_of_docs(number_of_docs)
	, number_of_batches(number_of_batches)
{
}

BatchSender::~BatchSender()
{
	formAndSend();
}

void BatchSender::send(ubjson::Value val)
{
	if (number_of_batches <= 0) {
		return;
	}

	queue.push(val);
	++sent;
	BOOST_LOG_TRIVIAL(trace) << "Something is pushed to send queue. Queue size: " << queue.size() << ". ";

	if (queue.size() >= number_of_docs) {
		formAndSend();
	}
}

void BatchSender::formAndSend()
{
	BOOST_LOG_TRIVIAL(trace) << "There are enough data to send, making batch…";
	ubjson::StreamWriter<SocketStream> writer(stream);

	ubjson::Value batch;

	for (int i = 0; i < number_of_docs && !queue.empty(); ++i) {
		batch["docs"].push_back(queue.front());
		queue.pop();
	}

	BOOST_LOG_TRIVIAL(trace) << "Batch is done, sending";
	writer.writeValue(batch);
	--number_of_batches;
	BOOST_LOG_TRIVIAL(trace) << "Batch is sent";
}
