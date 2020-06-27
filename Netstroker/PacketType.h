#pragma once
#include <stdint.h>

namespace Netstroker
{
	enum PacketType : uint16_t
	{
		PT_Invalid,
		PT_String,
		PT_Handshake
	};
}