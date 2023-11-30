#include "net_server.hxx"

using namespace std;

class CustomServer : public net_server<MsgTypes>
{
public:
	CustomServer(uint16_t nPort) : net_server<MsgTypes>(nPort, 10)
	{
	}

protected:
	
};

int main()
{
	CustomServer server(60000);
	server.Start();

	return 0;
}
