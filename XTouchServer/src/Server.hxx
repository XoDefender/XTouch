#include "net_server.hxx"

class Server : public net_server<MsgTypes>
{
public:
	Server(uint16_t nPort);
	void ConnectToMySQL(std::string ip, std::string name, std::string password, std::string schema);
};
