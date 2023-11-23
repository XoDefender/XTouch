#include <iostream>
#include <string>
#include <vector>

class SessionManager
{
public:
    SessionManager(SessionManager &) = delete;
    void operator=(const SessionManager &) = delete;

    static SessionManager *GetInstance();

    void RegisterUser(std::string ip, int port);

private:
    SessionManager();

private:
    struct SessionUser
    {
        std::string name;
        std::string password;
        std::string group;
        std::string ip;
        int port;
        std::string activeModel;
    };
    
    static SessionManager *instance;
    std::vector<SessionUser> sessionUsers;
};