#include "Network.h"


bool Netstroker::Network::Initialize()
{
	WSADATA wsadata;
	int result = WSAStartup(MAKEWORD(2,2), &wsadata);
	if (result != 0)
	{
		std::cerr << "Failed to start winsock API." << std::endl;
		return false;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
	{
		std::cerr << "Winsock Version 2.2 not found." << std::endl;
		return false;
	}
	return true; 

	
}

void Netstroker::Network::Shutdown()
{
	WSACleanup();
}
