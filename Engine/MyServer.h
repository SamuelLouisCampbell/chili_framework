#pragma once
#include <Netstroker/IncludeMe.h>

using namespace Netstroker;

class MyServer : public Server
{
public:
	void OnConnect(TCPConnection& newConnection) override;
	void OnDisconnect(TCPConnection& lostConnection, std::string reason) override;
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
	std::string GetMsg() const; 
	std::string GetEscCode() const;

private:
	std::string message;
	std::string escapeCode; 

};