#include "Client.h"
#include <iostream>

namespace Netstroker
{
	bool Netstroker::Client::Connect(IPEndpoint ip)
	{	
		isConnected = false;
		Socket socket = Socket(ip.GetIPVersion());
		socket = Socket(ip.GetIPVersion());
		if (socket.Create() == PResult::P_Success)
		{
			if (socket.SetBlocking(true) != PResult::P_Success)
				return false;
	
			std::cout << "Socket created OK" << std::endl;
			if (socket.Connect(ip) == PResult::P_Success)
			{
				if (socket.SetBlocking(false) == PResult::P_Success)
				{
					connection = TCPConnection(socket, ip);
					master_fd.fd = connection.socket.GetHandle();
					master_fd.events = POLLRDNORM;
					master_fd.revents = 0;
					isConnected = true;
					OnConnect();
					return true;
				}
			}
			else
			{
			}
			socket.Close();
		}
		else
		{
			std::cerr << "Socket creation FAIL" << std::endl;
		}
		OnConnectFail();
		return false;
		
	}

	bool Netstroker::Client::IsConnected() const
	{
		return isConnected;
	}

	bool Netstroker::Client::Frame()
	{
		if (connection.pm_outgoing.HasPendingPackets())
		{
			master_fd.events = POLLRDNORM | POLLWRNORM;
		}
		use_fd = master_fd;

		if (WSAPoll(&use_fd, 1, 1) > 0)
		{
			if (use_fd.revents & POLLERR)
			{
				CloseConnection( "Poll Error");
				return false;
			}
			if (use_fd.revents & POLLHUP)
			{
				CloseConnection( "Poll Hangup");
				return false;
			}
			if (use_fd.revents & POLLNVAL)
			{
				CloseConnection( "Poll invadlid");
				return false;
			}
			if (use_fd.revents & POLLRDNORM)
			{

				int bytesRecvd = 0;

				if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
				{
					bytesRecvd = recv(use_fd.fd, (char*)&connection.pm_incoming.currentPacketSize + connection.pm_incoming.currentPacketExtractionOffset,
						sizeof(uint16_t) - connection.pm_incoming.currentPacketExtractionOffset,
						0);
				}
				else
				{
					bytesRecvd = recv(use_fd.fd, (char*)&connection.buffer + connection.pm_incoming.currentPacketExtractionOffset,
						connection.pm_incoming.currentPacketSize - connection.pm_incoming.currentPacketExtractionOffset,
						0);
				}



				if (bytesRecvd == 0)//connection was lost.
				{
					CloseConnection( "Connection lost");
					return false;
				}

				if (bytesRecvd == SOCKET_ERROR)//connection was lost.
				{
					int error = WSAGetLastError();
					if (error != WSAEWOULDBLOCK)
					{
						CloseConnection( "Socket Error");
						return false;
					}


				}
				if (bytesRecvd > 0)
				{
					connection.pm_incoming.currentPacketExtractionOffset += bytesRecvd;
					if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
					{
						if (connection.pm_incoming.currentPacketExtractionOffset == sizeof(uint16_t))
						{
							connection.pm_incoming.currentPacketSize = ntohs(connection.pm_incoming.currentPacketSize);
							if (connection.pm_incoming.currentPacketSize > Netstroker::g_MaxPacketSize)
							{
								CloseConnection( "Packets are too big fucker!");
								return false;
							}
							connection.pm_incoming.currentPacketExtractionOffset = 0;
							connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketContents;
						}
					}
					else //process that packet dude.
					{
						if (connection.pm_incoming.currentPacketExtractionOffset == connection.pm_incoming.currentPacketSize)
						{
							std::shared_ptr<Packet> packet = std::make_shared<Packet>();
							packet->buffer.resize(connection.pm_incoming.currentPacketSize);
							memcpy(&packet->buffer[0], connection.buffer, connection.pm_incoming.currentPacketSize);

							connection.pm_incoming.Append(packet);

							connection.pm_incoming.currentPacketSize = 0;
							connection.pm_incoming.currentPacketExtractionOffset = 0;
							connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketSize;
						}
					}

				}

			}
			if (use_fd.revents & POLLWRNORM)
			{
				PacketManager& pm = connection.pm_outgoing;
				while (pm.HasPendingPackets())
				{
					if (pm.currentTask == PacketManagerTask::ProcessPacketSize) // sned packet size
					{
						pm.currentPacketSize = (uint16_t)pm.Retrieve()->buffer.size();
						uint16_t bigEndianPacketSize = htons(pm.currentPacketSize);
						int bytesSent = send(use_fd.fd, (char*)(&bigEndianPacketSize) + pm.currentPacketExtractionOffset,
							sizeof(uint16_t) - pm.currentPacketExtractionOffset, 0);
						if (bytesSent > 0)
						{
							pm.currentPacketExtractionOffset += bytesSent;
						}
						if (pm.currentPacketExtractionOffset == sizeof(uint16_t)) // if a full packet sent
						{
							pm.currentPacketExtractionOffset = 0;
							pm.currentTask = PacketManagerTask::ProcessPacketContents;
						}
						else
						{
							break;
						}

					}
					else // send packet contents
					{
						char* bufferPtr = &pm.Retrieve()->buffer[0];
						int bytesSent = send(use_fd.fd, (char*)(bufferPtr)+pm.currentPacketExtractionOffset,
							pm.currentPacketSize - pm.currentPacketExtractionOffset, 0);
						if (bytesSent > 0)
						{
							pm.currentPacketExtractionOffset += bytesSent;
						}
						if (pm.currentPacketExtractionOffset == pm.currentPacketSize) // if a full packet sent
						{
							pm.currentPacketExtractionOffset = 0;
							pm.currentTask = PacketManagerTask::ProcessPacketSize;
							pm.Pop(); //remove packet from queue
						}
						else
						{
							break;
						}
					}
				}
				if (!connection.pm_outgoing.HasPendingPackets())
				{
					master_fd.events = POLLRDNORM;
				}
			}

			while (connection.pm_incoming.HasPendingPackets())
			{
				std::shared_ptr<Packet> frontPacket = connection.pm_incoming.Retrieve();
				if (!ProcessPacket(frontPacket))
				{
					CloseConnection("Failure to do this fucking packet..");
					return false;
				}
				connection.pm_incoming.Pop();
			}
			
		
		}
	}

	bool Netstroker::Client::ProcessPacket(std::shared_ptr<Packet> packet)
	{
		std::cout << "Packet recieved with size: " << packet->buffer.size() << std::endl;
		return true;
	}

	void Netstroker::Client::OnConnect()
	{
		std::cout << "Connected Success!" << std::endl;
	}

	void Netstroker::Client::OnConnectFail()
	{

		std::cout << "Connected Fail!" << std::endl;
	}

	void Netstroker::Client::OnDisconnect(std::string reason)
	{
		std::cout << "[" << reason << "] - Connection lost ... " << std::endl;
	}
	void Client::AppendPacket(std::shared_ptr<Packet> packet)
	{
		connection.pm_outgoing.Append(packet);
	}
	void Client::CloseConnection(std::string reason)
	{
		OnDisconnect(reason);
		master_fd.fd = 0;
		isConnected = false;
		connection.Close();
	}
}