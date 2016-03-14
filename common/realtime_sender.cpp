#include "realtime_sender.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

RealTimeSender::RealTimeSender(SocketStream& stream)
	: SenderInterface(stream)
{
}

void RealTimeSender::send(ubjson::Value val)
{
	BOOST_LOG_TRIVIAL(trace) << "RealTime sender: sending doc….";
	sent++;
	ubjson::StreamWriter<SocketStream> writer(stream);
	writer.writeValue(val);
	BOOST_LOG_TRIVIAL(trace) << "RealTime sender: sent doc….";
}
