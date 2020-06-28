//Client Code
#include <NetStroker\IncludeMe.h>
#include "MyClient.h"
#include <conio.h>
using namespace Netstroker;

int main()
{


	if (Network::Initialize())
	{
		MyClient client;

		if (client.Connect(IPEndpoint("127.0.0.1", 8000)))
		{

			std::string message;

			while (client.IsConnected())
			{
				if (_kbhit())
				{
					std::shared_ptr<Packet> myPacket = std::make_shared<Packet>(PacketType::PT_String);
					char character = _getch();
					if (character == '\b' && message.size() > 1)
					{
						message.pop_back();
						*myPacket << message;
						client.AppendPacket(myPacket);
					}
					else if (character == '\b' && message.size() == 1)
					{
						message.clear();
						message.resize(0);
						*myPacket << message;
						client.AppendPacket(myPacket);
					}
					else
					{
						if (character >= ' ' && character <= '~')
						{
							message.push_back(character);
							*myPacket << message;
							client.AppendPacket(myPacket);
						}
						else if (character == '\r')
						{
							message.push_back(character);
							*myPacket << message;
							client.AppendPacket(myPacket);

						}
					}
					

				
				}

				client.Frame();


			}
		}
	}

	Network::Shutdown();
	system("pause");
	return 0;
}