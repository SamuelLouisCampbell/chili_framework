#include "MyServer.h"
#include <sstream>

void MyServer::OnConnect(TCPConnection& newConnection)
{
	std::stringstream infostring;
	infostring << newConnection.ToString() << " - New connection accepted... " << std::endl;
	OutputDebugStringA(infostring.str().c_str());

	std::shared_ptr<Packet> newUserPacket = std::make_shared<Packet>(PacketType::PT_Handshake);
	*newUserPacket << std::string("New User Connected");
	for (auto& c : connections)
	{
		if (&c == &newConnection)
			continue;

		c.pm_outgoing.Append(newUserPacket);
	}
}

void MyServer::OnDisconnect(TCPConnection& lostConnection, std::string reason)
{
	std::stringstream infostring;
	infostring << "[" << reason << "] - Connection lost ... " << lostConnection.ToString() << std::endl;
	OutputDebugStringA(infostring.str().c_str());

	std::shared_ptr<Packet> lostUserPacket = std::make_shared<Packet>(PacketType::PT_Handshake);
	*lostUserPacket << std::string("This user: ") << lostConnection.ToString() << std::string("disconnected");
	for (auto& c : connections)
	{
		if (&c == &lostConnection)
			continue;

		c.pm_outgoing.Append(lostUserPacket);
	}

}

bool MyServer::ProcessPacket(std::shared_ptr<Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PacketType::PT_String:
	{
		std::stringstream infostring;

		*packet >> message;
		std::string echoPre = "Echo from server: " + message;
		infostring << ">>>" << echoPre << std::endl;		
		std::shared_ptr<Packet> echo = std::make_shared<Packet>(PacketType::PT_String);
	
		*echo << echoPre;
		for (auto& c : connections)
		{
			c.pm_outgoing.Append(echo);
		}
	
		OutputDebugStringA(infostring.str().c_str());
		break;
	}
	case PacketType::PT_Handshake:
	{
		std::string message;
		*packet >> message;
		OutputDebugStringA(message.c_str() + '\n');
		break;
	}
	case PacketType::PT_EscapeCode:
	{
		*packet >> escapeCode;
		OutputDebugStringA("NEWLINE!\n");
		break;
	}
	default:
	{
		std::stringstream infostring;
		infostring << "Unrecognised Packet" << std::endl;
		OutputDebugStringA(infostring.str().c_str());
		return false;
	}

	}
	return true;
}

std::string MyServer::GetMsg() const
{
	return message;
}

std::string MyServer::GetEscCode() const
{
	return escapeCode;
}
