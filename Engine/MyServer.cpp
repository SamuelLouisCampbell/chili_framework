#include "MyServer.h"

void MyServer::OnConnect(TCPConnection& newConnection)
{
	std::cout << newConnection.ToString() << " - New connection accepted... " << std::endl;
	
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
	std::cout << "[" << reason << "] - Connection lost ... " << lostConnection.ToString() << std::endl;

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
		
		*packet >> message;
		std::string echoPre = "Echo from server: " + message;
		std::cout << ">>>" << message << std::endl;		
		std::shared_ptr<Packet> echo = std::make_shared<Packet>(PacketType::PT_String);
	
		*echo << echoPre;
		for (auto& c : connections)
		{
			c.pm_outgoing.Append(echo);
		}

		break;
	}
	case PacketType::PT_Handshake:
	{
		std::string message;
		*packet >> message;
		std::cout << message << std::endl;
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

std::string MyServer::GetMsg() const
{
	return message;
}
