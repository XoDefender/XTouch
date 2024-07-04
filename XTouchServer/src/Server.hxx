#include "net_server.hxx"

class Server : public net_server<MsgTypes>
{
public:
	Server(uint16_t nPort, std::string res_dir_path);
	void ConnectToMySQL(std::string ip, std::string name, std::string password, std::string schema);
};
