#include "Network.h"


bool Netstroker::Network::Initialize()
{
	WSADATA wsadata;
	int result = WSAStartup(MAKEWORD(2,2), &wsadata);
	if (result != 0)
	{
		OutputDebugString(L"Failed to start winsock API.\n");
		return false;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
	{
		OutputDebugString(L"Winsock Version 2.2 not found.\n");
	}
	return true; 

	
}

void Netstroker::Network::Shutdown()
{
	WSACleanup();
}
