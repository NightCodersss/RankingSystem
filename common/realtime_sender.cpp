#include "realtime_sender.hpp"

void RealTimeSender::send(SocketStream& stream, ubjson::Value val)
{
	ubjson::StreamWriter<SocketStream> writer(stream);
	writer.writeValue(val);
}
