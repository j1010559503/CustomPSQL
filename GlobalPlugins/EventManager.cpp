#include "EventManager.h"

EventManager& EventManager::instance()
{
    static EventManager s_instance;
    return s_instance;
}
