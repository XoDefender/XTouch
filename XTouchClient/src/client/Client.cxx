#include "Client.hxx"
#include "../main/global.hxx"

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

net::message<MsgTypes> Client::SendRequestToServer(MsgTypes msgType, net::message<MsgTypes> msg)
{
    SendMessage(msg, msgType);
    return StartMessageLoop();
}

net::message<MsgTypes> Client::StartMessageLoop()
{
    while (1)
    {
        int activeEvents = epoll_wait(efd, events, 1, -1);
        for (int i = 0; i < activeEvents; i++)
        {
            net::message<MsgTypes> omsg;
            if(ReadMessage(clientfd, omsg) == -1)
            {
                std::cout<<"Error reading the message\n";
            }
            else return omsg;
        }
    }
}

net::message<MsgTypes> Client::GetEmptyMessage()
{
    return emptyMsg;
}

MsgTypes operator|(MsgTypes first_type, MsgTypes second_type)
{
    using underlying_type = std::underlying_type_t<MsgTypes>;
    return static_cast<MsgTypes>(static_cast<underlying_type>(first_type) | static_cast<underlying_type>(second_type));
}