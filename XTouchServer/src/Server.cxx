#include "Server.hxx"

Server::Server(uint16_t nPort) : net_server<MsgTypes>(nPort, 10){};

void Server::ConnectToMySQL(string ip, string name, string password, string schema)
{
	Driver *driver = get_driver_instance();
	Connection *con = driver->connect(ip, name, password);
	con->setSchema(schema);
	stmt = con->createStatement();
}
