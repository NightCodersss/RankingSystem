#pragma once

#include "sender.hpp"

class RealTimeSender : public SenderInterface
{
public:
	RealTimeSender(SocketStream& stream); 
	void send(ubjson::Value val) override;
};
