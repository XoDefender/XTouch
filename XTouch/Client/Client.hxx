#pragma once
#include "ocl_net.hxx"

enum class MsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,

	PasswordLogin,

	GetModels,
	GetModelFiles,

	AddModelToFavorite,
	RemoveModelFromFavorite,

	GetModelByName,
	GetFavoriteModels,

	SortModels,
	SortFavoriteModels,
	SortFiles,

	GetModelFile,
};

class Client : public olc::net::client_interface<MsgTypes>
{
public:
	olc::net::message<MsgTypes> SendRequestToServer(MsgTypes msgType, olc::net::message<MsgTypes> msg = Client::GetInstance().GetEmptyMessage());
    olc::net::message<MsgTypes> GetEmptyMessage();

public:
	static Client &GetInstance();
	Client(const Client &) = delete;

private:
	Client();
    ~Client();

private:
	olc::net::message<MsgTypes> StartMessageLoop();
    olc::net::message<MsgTypes> emptyMsg;
};