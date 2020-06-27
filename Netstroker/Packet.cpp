#include "Packet.h"
#include "Constants.h"


namespace Netstroker
{
	Packet::Packet(PacketType packetType)
	{
		Clear();
		AssignPacketType(packetType);
	}
	PacketType Packet::GetPacketType()
	{
		PacketType* pPacketType = reinterpret_cast<PacketType*>(&buffer[0]);
		return  static_cast<PacketType>(ntohs(*pPacketType));
	}
	void Packet::AssignPacketType(PacketType packetType)
	{
		PacketType* pPacketType = reinterpret_cast<PacketType*>(&buffer[0]);
		*pPacketType = static_cast<PacketType>(htons(packetType));
	}
	void Packet::Clear()
	{
		buffer.resize(sizeof(PacketType));
		AssignPacketType(PacketType::PT_Invalid);
		extractionOffset = sizeof(PacketType);
	}

	void Packet::Append(const void* data, uint32_t size)
	{
		if ((buffer.size() + size) > Netstroker::g_MaxPacketSize)
		{
			throw PacketException("[void Packet::Append(const void* data, uint32_t size)] - Appended packet size greater than g_MaxPacketSize");
		}
		buffer.insert(buffer.end(), (char*)data, (char*)data + size);
	}

	Packet& Packet::operator<<(uint32_t data)
	{
		data = htonl(data);
		Append(&data, sizeof(uint32_t));
		return *this;

	}

	Packet& Packet::operator>>(uint32_t& data)
	{
		if((extractionOffset + sizeof(uint32_t)) > buffer.size())
		{
			throw PacketException("[Packet& Packet::operator>>(uint32_t& data)] - Extracted packet exceeded buffer size");
		}
		data = *reinterpret_cast<uint32_t*>(&buffer[extractionOffset]);
		data = ntohl(data);
		extractionOffset += sizeof(uint32_t);
		return *this;
	}

	Packet& Packet::operator<<(const std::string& data)
	{
		*this << (uint32_t)data.size();
		Append(data.data(), data.size());
		return *this;
	}

	Packet& Packet::operator>>(std::string& data)
	{
		data.clear();
		uint32_t stringSize = 0;
		*this >> stringSize;

		if ((extractionOffset + stringSize) > buffer.size())
		{
			throw PacketException("[Packet& Packet::operator>>(std::string& data)] - Extracted packet exceeded buffer size");
		}

		data.resize(stringSize);
		data.assign(&buffer[extractionOffset], stringSize);
		extractionOffset += stringSize;

		return* this;
	}

}
