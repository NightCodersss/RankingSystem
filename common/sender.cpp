#include "sender.hpp"

SenderInterface::SenderInterface(SocketStream& stream)
	: stream(stream)
{
}
