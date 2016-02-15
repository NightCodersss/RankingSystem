#include "realtime_sender.hpp"

RealTimeSender::RealTimeSender(SocketStream& stream)
	: SenderInterface(stream)
{
}

void RealTimeSender::send(ubjson::Value val)
{
	ubjson::StreamWriter<SocketStream> writer(stream);
	writer.writeValue(val);
}
