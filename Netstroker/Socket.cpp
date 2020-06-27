#include "Socket.h"
#include <iostream>
#include <assert.h>

namespace Netstroker
{
	Netstroker::Socket::Socket(IPVersion ipversion, SocketHandle handle)
		:
		ipversion(ipversion),
		handle(handle)
	{
		assert(ipversion == IPVersion::IPv4 || ipversion == IPVersion::IPv6);
	}

	PResult Netstroker::Socket::Create()
	{
		assert(ipversion == IPVersion::IPv4 || ipversion == IPVersion::IPv6);
		if (handle != INVALID_SOCKET)
		{
			return PResult::P_GenericError;
		}
		handle = socket((ipversion == IPVersion::IPv4) ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_TCP);

		if (handle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		if (SetBlocking(false) != PResult::P_Success)
		{
			return PResult::P_GenericError;
		}

		if (SetSocketOptions(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success)
		{
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Netstroker::Socket::Close()
	{
		if (handle == INVALID_SOCKET)
		{
			return PResult::P_GenericError;
		}

		int result = closesocket(handle);
		if (result != 0)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		handle = INVALID_SOCKET;
		return PResult::P_Success;

	}

	PResult Socket::Bind(IPEndpoint endpoint)
	{
		assert(ipversion == endpoint.GetIPVersion());

		if (ipversion == IPVersion::IPv4)
		{
			sockaddr_in addr = endpoint.GetSockaddrIPv4();
			int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
			if (result != 0)
			{
				int error = WSAGetLastError();
				return PResult::P_GenericError;
			}
		}
		else
		{
			sockaddr_in6 addr = endpoint.GetSockaddrIPv6();
			int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in6));
			if (result != 0)
			{
				int error = WSAGetLastError();
				return PResult::P_GenericError;
			}
		}
		return PResult::P_Success;
	}

	PResult Socket::Listen(IPEndpoint endpoint, int backlog)
	{

		if (ipversion == IPVersion::IPv6)
		{
			if (SetSocketOptions(SocketOption::IPV6_Only, FALSE) != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
		}

		if (Bind(endpoint) != PResult::P_Success)
		{
			return PResult::P_GenericError;
		}

		int result = listen(handle, backlog);
		if (result != 0)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::Accept(Socket& outSocket, IPEndpoint* endpoint)
	{
		assert(ipversion == IPVersion::IPv4 || ipversion == IPVersion::IPv6);
		if (ipversion == IPVersion::IPv4)
		{
			sockaddr_in addr = {};
			int len = sizeof(sockaddr_in);
			SocketHandle acceptedConncetionHandle = accept(handle, (sockaddr*)(&addr), &len);
			if (acceptedConncetionHandle == INVALID_SOCKET)
			{
				int error = WSAGetLastError();
				return PResult::P_GenericError;
			}
			if (endpoint != nullptr)
			{
				*endpoint = IPEndpoint((sockaddr*)&addr);
			}
			IPEndpoint newConnectionEndpoint((sockaddr*)&addr);
			//std::cout << "New connection accepted!" << std::endl;
			//newConnectionEndpoint.Print();
			
			outSocket = Socket(IPVersion::IPv4, acceptedConncetionHandle);
		}
		else
		{
			sockaddr_in6 addr = {};
			int len = sizeof(sockaddr_in6);
			SocketHandle acceptedConncetionHandle = accept(handle, (sockaddr*)(&addr), &len);
			if (acceptedConncetionHandle == INVALID_SOCKET)
			{
				int error = WSAGetLastError();
				return PResult::P_GenericError;
			}
			if (endpoint != nullptr)
			{
				*endpoint = IPEndpoint((sockaddr*)&addr);
			}
			IPEndpoint newConnectionEndpoint((sockaddr*)&addr);
			//std::cout << "New connection accepted!" << std::endl;
			//newConnectionEndpoint.Print();

			outSocket = Socket(IPVersion::IPv6, acceptedConncetionHandle);
		}
	
		return PResult::P_Success;
	}

	PResult Socket::Connect(IPEndpoint endpoint)
	{
		assert(ipversion == endpoint.GetIPVersion());
		int result = 0;
		if (ipversion == IPVersion::IPv4)
		{
			sockaddr_in addr = endpoint.GetSockaddrIPv4();
			int result = connect(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		
		}
		else
		{
			sockaddr_in6 addr = endpoint.GetSockaddrIPv6();
			int result = connect(handle, (sockaddr*)(&addr), sizeof(sockaddr_in6));
		}
		if (result != 0)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Send(const void* data, int numberOfBytes, int& bytesSent)
	{
		bytesSent = send(handle, (const char*)data, numberOfBytes, 0);

		if (bytesSent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Recv(void* destination, int numberOfBytes, int& bytesRecieved)
	{
		bytesRecieved = recv(handle, (char*)destination, numberOfBytes, 0);
		if (bytesRecieved == 0)
		{
			return PResult::P_GenericError;
		}
		if (bytesRecieved == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		
		return PResult::P_Success;
	}

	PResult Socket::SendAll(const void* data, int numberOfBytes)
	{
		int totalBytesSent = 0;

		while (totalBytesSent < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesSent;
			int bytesSent = 0;
			char* bufferOffset = (char*)data + totalBytesSent;
			PResult result = Send(bufferOffset, bytesRemaining, bytesSent);
			if (result != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
			totalBytesSent = bytesSent;
		}
		return PResult::P_Success;
	}

	PResult Socket::RecvAll(void* destination, int numberOfBytes)
	{
		int totalBytesRecvd = 0;

		while (totalBytesRecvd < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesRecvd;
			int bytesRecvd = 0;
			char* bufferOffset = (char*)destination + totalBytesRecvd;
			PResult result = Recv(bufferOffset, bytesRemaining, bytesRecvd);
			if (result != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
			totalBytesRecvd = bytesRecvd;
		}
		return PResult::P_Success;
	}

	PResult Socket::Send(Packet& packet)
	{
		uint16_t encodedPacketSize = htons((uint16_t)packet.buffer.size());
		PResult result = SendAll(&encodedPacketSize, sizeof(uint16_t));
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		result = SendAll(packet.buffer.data(), packet.buffer.size());
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		return PResult::P_Success;
	}

	PResult Socket::Recv(Packet& packet)
	{
		packet.Clear();
		uint16_t encodedSize = 0;
		PResult result = RecvAll(&encodedSize, sizeof(uint16_t));
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		uint16_t bufferSize = ntohs(encodedSize);

		if (bufferSize > Netstroker::g_MaxPacketSize)
			return PResult::P_GenericError;
		

		packet.buffer.resize(bufferSize);
		result = RecvAll(&packet.buffer[0], bufferSize);
		if (result != PResult::P_Success)
			return PResult::P_GenericError;


		return PResult::P_Success;
	
	}

	SocketHandle Netstroker::Socket::GetHandle()
	{
		return handle;
	}

	IPVersion Netstroker::Socket::GetIPVersion()
	{
		return ipversion;
	}

	PResult Socket::SetBlocking(bool isBlocking)
	{
		unsigned long nonBlocking = 1;
		unsigned long blocking = 0;
		int result = ioctlsocket(handle, FIONBIO, isBlocking ? &blocking : &nonBlocking);
		if (result == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::SetSocketOptions(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
			break;
		case SocketOption::IPV6_Only:
			result = setsockopt(handle, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&value, sizeof(value));
			break;
		default:
			return PResult::P_GenericError;
		}

		if (result != 0)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}
}