#pragma once
#include "Socket.h"
#include "PacketManager.h"

namespace Netstroker
{
	class TCPConnection
	{
	public:
		TCPConnection(Socket socket, IPEndpoint endpoint);
		TCPConnection() :socket(Socket()) {};
		void Close();
		std::string ToString() const;
		Socket socket;

		PacketManager pm_incoming;
		PacketManager pm_outgoing;

		char buffer[Netstroker::g_MaxPacketSize];
	private:
		IPEndpoint endpoint;
		std::string stringRepresentation = "";
	};
}