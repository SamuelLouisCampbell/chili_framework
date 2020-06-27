#include "Server.h"
#include "Network.h"
#include <sstream>

namespace Netstroker
{
	bool Server::Initialize(IPEndpoint ip)
	{
		master_fd.clear();
		connections.clear();

		std::stringstream infostream;
		infostream << "Winsock API initialized." << std::endl;
		OutputDebugStringA(infostream.str().c_str());

		listeningSocket = Socket(ip.GetIPVersion());
		if (listeningSocket.Create() == PResult::P_Success)
		{
			if (listeningSocket.Listen(ip) == PResult::P_Success)
			{
				WSAPOLLFD listeningSocketFD = {};
				listeningSocketFD.fd = listeningSocket.GetHandle();
				listeningSocketFD.events = POLLRDNORM;
				listeningSocketFD.revents = 0;

				master_fd.push_back(listeningSocketFD);
				
				std::stringstream infostream;
				infostream << "Listening on port: " << ip.GetPort() << std::endl;
				OutputDebugStringA(infostream.str().c_str());
				return true;
			}
			else
			{
				std::stringstream errorstream;
				errorstream << "Failed binding to port: " << ip.GetPort() << std::endl;
				OutputDebugStringA(errorstream.str().c_str());
				
			}
			listeningSocket.Close();
		}
		else
		{
			OutputDebugString(L"Socket creation FAIL\n");
		}
		return false;
	}

	void Server::Frame()
	{
		for (int i = 0; i < connections.size(); i++)
		{
			if (connections[i].pm_outgoing.HasPendingPackets())
			{
				master_fd[i + 1].events = POLLRDNORM | POLLWRNORM;
			}
		}
		use_fd.clear();
		use_fd = master_fd;

		if (WSAPoll(use_fd.data(), use_fd.size(), 1) > 0)
		{
#pragma region listener
			WSAPOLLFD& listeningSocketFD = use_fd[0];
			if (listeningSocketFD.revents & POLLRDNORM)
			{
				Socket newConnectionSocket;
				IPEndpoint newConnectionEndpoint;
				if (listeningSocket.Accept(newConnectionSocket, &newConnectionEndpoint) == PResult::P_Success)
				{
					connections.emplace_back(TCPConnection(newConnectionSocket, newConnectionEndpoint));
					TCPConnection& acceptedConnection = connections[connections.size() - 1];

					WSAPOLLFD newConnectionFD = {};
					newConnectionFD.fd = newConnectionSocket.GetHandle();
					newConnectionFD.events = POLLRDNORM;
					newConnectionFD.revents = 0;
					master_fd.push_back(newConnectionFD);
					OnConnect(acceptedConnection);
				}
				else
				{
					OutputDebugString(L"Failed to accept new connection.\n");
				}
			}
#pragma endregion Code specific to the listning socket.

			for (int i = use_fd.size() - 1; i >= 1; i--)
			{
				int connectionIndex = i - 1;

				TCPConnection& connection = connections[connectionIndex];
				if (use_fd[i].revents & POLLERR)
				{
					CloseConnection(connectionIndex, "Poll Error");
					continue;
				}
				if (use_fd[i].revents & POLLHUP)
				{
					CloseConnection(connectionIndex, "Poll Hangup");
					continue;
				}
				if (use_fd[i].revents & POLLNVAL)
				{
					CloseConnection(connectionIndex, "Poll invadlid");
					continue;
				}
				if (use_fd[i].revents & POLLRDNORM)
				{

					int bytesRecvd = 0;

					if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
					{
						bytesRecvd = recv(use_fd[i].fd, (char*)&connection.pm_incoming.currentPacketSize + connection.pm_incoming.currentPacketExtractionOffset,
							sizeof(uint16_t) - connection.pm_incoming.currentPacketExtractionOffset,
							0);
					}
					else
					{
						bytesRecvd = recv(use_fd[i].fd, (char*)&connection.buffer + connection.pm_incoming.currentPacketExtractionOffset,
							connection.pm_incoming.currentPacketSize - connection.pm_incoming.currentPacketExtractionOffset,
							0);
					}



					if (bytesRecvd == 0)//connection was lost.
					{
						CloseConnection(connectionIndex, "Connection lost");
						continue;
					}

					if (bytesRecvd == SOCKET_ERROR)//connection was lost.
					{
						int error = WSAGetLastError();
						if (error != WSAEWOULDBLOCK)
						{
							CloseConnection(connectionIndex, "Socket Error");
							continue;
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
									CloseConnection(connectionIndex, "Packets are too big fucker!");
									continue;
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
				if (use_fd[i].revents & POLLWRNORM)
				{
					PacketManager& pm = connection.pm_outgoing;
					while (pm.HasPendingPackets())
					{
						if (pm.currentTask == PacketManagerTask::ProcessPacketSize) // sned packet size
						{
							pm.currentPacketSize = pm.Retrieve()->buffer.size();
							uint16_t bigEndianPacketSize = htons(pm.currentPacketSize);
							int bytesSent = send(use_fd[i].fd, (char*)(&bigEndianPacketSize) + pm.currentPacketExtractionOffset,
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
							int bytesSent = send(use_fd[i].fd, (char*)(bufferPtr)+pm.currentPacketExtractionOffset,
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
					if (!pm.HasPendingPackets())
					{
						master_fd[i].events = POLLRDNORM;
					}
				}

				for (int i = connections.size() - 1; i >= 0; i--)
				{
					while (connections[i].pm_incoming.HasPendingPackets())
					{
						std::shared_ptr<Packet> frontPacket = connections[i].pm_incoming.Retrieve();
						//std::cout << "Packet recieved with size: " << frontPacket->buffer.size() << std::endl;
						if (!ProcessPacket(frontPacket))
						{
							CloseConnection(i, "Failure to do this fucking packet..");
							break;
						}
						connections[i].pm_incoming.Pop();
					}
				}
			}
		}


	}

	void Server::OnConnect(TCPConnection& newConnection)
	{
		std::stringstream infostring;
		infostring << newConnection.ToString() << " - New connection accepted... " << std::endl;
		OutputDebugStringA(infostring.str().c_str());
	}

	void Server::OnDisconnect(TCPConnection& lostConnection, std::string reason)
	{
		std::stringstream infostring;
		infostring << "[" << reason << "] - Connection lost ... " << lostConnection.ToString() << std::endl;
		OutputDebugStringA(infostring.str().c_str());
	}

	void Server::CloseConnection(int connectionIndex, std::string reason)
	{
		TCPConnection& connection = connections[connectionIndex];
		OnDisconnect(connection, reason);
		master_fd.erase(master_fd.begin() + (connectionIndex + 1));
		use_fd.erase(use_fd.begin() + (connectionIndex + 1));
		connection.Close();
		connections.erase(connections.begin() + connectionIndex);
	}

	bool Server::ProcessPacket(std::shared_ptr<Packet> packet)
	{
		std::stringstream infostring;
		infostring << "Packet recieved with size: " << packet->buffer.size() << std::endl;
		OutputDebugStringA(infostring.str().c_str());
		return true;
	}

}