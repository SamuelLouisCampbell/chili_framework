#include <WinSock2.h>
#include <iostream>

namespace Netstroker
{
	class Network
	{
	public:
		static bool Initialize();
		static void Shutdown();
	};
}