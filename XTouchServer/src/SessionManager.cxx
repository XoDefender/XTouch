#include "SessionManager.hxx"

SessionManager* SessionManager::instance = nullptr;

SessionManager* SessionManager::GetInstance()
{
    if(!instance) instance = new SessionManager();
    return instance;
}

SessionManager::SessionManager(){};

void SessionManager::RegisterUser(std::string ip, int port)
{
    SessionUser user;
    user.ip = ip;
    user.port = port;
}