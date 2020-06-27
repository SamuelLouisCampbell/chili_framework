#include "TCPConnection.h"

namespace Netstroker
{
	TCPConnection::TCPConnection(Socket socket, IPEndpoint endpoint)
		:
		socket(socket),
		endpoint(endpoint)
	{
		stringRepresentation = "[" + endpoint.GetIPString() + " : " + std::to_string(endpoint.GetPort()) + "]";
	}
	
	void TCPConnection::Close()
	{
		socket.Close();
	}
	
	std::string TCPConnection::ToString() const
	{
		return stringRepresentation;
	}
}