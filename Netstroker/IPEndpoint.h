#pragma once
#include "IPVersion.h"
#include <string>
#include <vector>
#include <WS2tcpip.h>

namespace Netstroker
{
	class IPEndpoint
	{
	public:
		IPEndpoint() {};
		IPEndpoint(const char* ip, unsigned short port);
		IPEndpoint(sockaddr* addr);
		IPVersion GetIPVersion() const;
		std::vector<uint8_t> GetIPBytes() const;
		std::string GetHostname() const;
		std::string GetIPString() const;
		unsigned short GetPort() const;
		sockaddr_in GetSockaddrIPv4();
		sockaddr_in6 GetSockaddrIPv6();
		void Print() const;

	private:
		IPVersion ipversion = IPVersion::Unkown;
		std::string hostname = "";
		std::string ip_string = "";
		std::vector<uint8_t> ip_bytes;
		unsigned short port = 0;
	};
}