#pragma once

#include "sender.hpp"

class RealTimeSender : public SenderInterface
{
public:
	void send(SocketStream& stream, ubjson::Value val) override;
};
