#include "Client.hxx"
#include "../global.hxx"

using namespace std;

Client::Client(){};

Client::~Client()
{
    Disconnect();
}

Client &Client::GetInstance()
{
    static Client client;
    return client;
}

olc::net::message<MsgTypes> Client::SendRequestToServer(MsgTypes msgType, olc::net::message<MsgTypes> msg)
{
    msg.header.id = msgType;

    Send(msg);

    return StartMessageLoop();
}

olc::net::message<MsgTypes> Client::StartMessageLoop()
{
    while (true)
    {
        if (IsConnected())
        {
            if (!Incoming().empty())
            {
                auto msg = Incoming().pop_front().msg;

                if (msg.size() <= 0) continue;
                else
                {
                    return msg;
                }
            }
        }
        else
        {
            Disconnect();
            throw std::string("The server is not active!");
        }
    }
}

olc::net::message<MsgTypes> Client::GetEmptyMessage()
{
    return emptyMsg;
}