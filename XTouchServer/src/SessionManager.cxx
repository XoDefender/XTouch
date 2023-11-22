#include "SessionManager.hxx"

SessionManager* SessionManager::GetInstance()
{
    if(!instance) instance = new SessionManager();
    return instance;
}