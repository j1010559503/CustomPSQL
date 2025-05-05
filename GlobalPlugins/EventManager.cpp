#include "EventManager.h"

EventManager* EventManager::s_instance = nullptr;

EventManager* EventManager::instance()
{
    if (s_instance == nullptr)
    {
        s_instance = new EventManager();
    }
    return s_instance;
}
