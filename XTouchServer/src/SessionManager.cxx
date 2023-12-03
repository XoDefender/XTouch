#include "SessionManager.hxx"

SessionManager *SessionManager::instance = nullptr;

SessionManager *SessionManager::GetInstance()
{
    if (!instance)
        instance = new SessionManager();
    return instance;
}

SessionManager::SessionManager(){};

void SessionManager::RegisterUser(std::string ip, int port, int fd)
{
    SessionUser user;
    user.ip = ip;
    user.port = port;
    user.fd = fd;
    sessionUsers.push_back(user);
}

void SessionManager::UnregisterUser(std::string ip, int port)
{
    sessionUsers.erase(remove_if(sessionUsers.begin(), sessionUsers.end(), [ip, port](const SessionUser &user)
                                 { return (user.ip == ip && user.port == port); }),
                       sessionUsers.end());
}

SessionUser *SessionManager::GetUser(std::string ip, int port)
{
    for (auto it = sessionUsers.begin(); it != sessionUsers.end(); ++it)
    {
        const SessionUser &user = *it;
        if (user.ip == ip && user.port == port) return &(*it);
    }

    return nullptr;
}