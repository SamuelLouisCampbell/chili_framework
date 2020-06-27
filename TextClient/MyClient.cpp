#include "MyClient.h"

bool Netstroker::MyClient::ProcessPacket(std::shared_ptr<Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PacketType::PT_String:
	{
		std::string message;
		*packet >> message;
		std::cout << message << std::endl;
		break;
	}
	case PacketType::PT_Handshake:
	{
		std::string message;
		*packet >> message;
		std::cout << "Handshake info: " << message << std::endl;
		break;
	}
	case PacketType::PT_EscapeCode:
	{
		std::string escapeCode;
		*packet >> escapeCode;
		OutputDebugStringA(escapeCode.c_str() + '\n');
		break;
	}
	default:
	{
		std::cout << "Unrecognised Packet" << std::endl;
		return false;
	}

	}
	return true;
}

void Netstroker::MyClient::OnConnect()
{
	std::cout << "Connected to Server" << std::endl;

	std::shared_ptr<Packet> newUserPacket = std::make_shared<Packet>(PacketType::PT_Handshake);
	*newUserPacket << std::string("Client logged on...");
	connection.pm_outgoing.Append(newUserPacket);
}

void Netstroker::MyClient::AppendPacket(std::shared_ptr<Packet> packet)
{
	//std::cout << "Sending user packet" << std::endl;
	connection.pm_outgoing.Append(packet);
}
