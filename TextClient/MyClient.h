#pragma once
#include<Netstroker/IncludeMe.h>

namespace Netstroker
{
	class MyClient : public Client
	{
	public:

		bool ProcessPacket(std::shared_ptr<Packet> packet) override;
		void OnConnect() override;
		void AppendPacket(std::shared_ptr<Packet> packet) override;

	private:
	
	};

}