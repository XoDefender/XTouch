#pragma once

#include "net_modules.hxx"

class Client : public net::net_client<MsgTypes>
{
public:
	net::message<MsgTypes> SendRequestToServer(MsgTypes msgType, net::message<MsgTypes> msg = Client::GetInstance().GetEmptyMessage());
    net::message<MsgTypes> GetEmptyMessage();

public:
	static Client &GetInstance();
	Client(const Client &) = delete;

private:
	Client();
    ~Client();

private:
	net::message<MsgTypes> StartMessageLoop();
    net::message<MsgTypes> emptyMsg;
};