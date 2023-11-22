#include <string>
#include <vector>

class SessionManager
{
public:
    SessionManager(SessionManager &) = delete;
    void operator=(const SessionManager &) = delete;

    SessionManager *GetInstance();

private:
    SessionManager();

private:
    struct SessionUser
    {
        std::string name;
        std::string password;
        std::string group;
        std::string ip;
        std::string activeModel;
    };
    
    static SessionManager *instance;
    std::vector<SessionUser> sessionUsers;
};