#include "server_interface.hxx"

using namespace std;

class CustomServer : public server_interface<MsgTypes>
{
public:
	CustomServer(uint16_t nPort) : server_interface<MsgTypes>(nPort, 10)
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
