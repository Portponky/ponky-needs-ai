#include "utility_server.h"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

//UtilityServer* UtilityServer::singleton = nullptr;

void UtilityServer::thread_func()
{
    uint64_t msdelay = 1000;

    while (!exit_thread)
    {
        //if (!ac->empty())
        {
            lock();
            //ac->register_rooms();
            unlock();
        }
        OS::get_singleton()->delay_usec(msdelay * 1000);
    }
}

void UtilityServer::_bind_methods()
{

}

Error UtilityServer::init()
{
    exit_thread = false;
    mutex = memnew(Mutex);
    thread = memnew(Thread);
    thread->reference(); // Fudge: The thread seems to get unreffed an extra time whilst starting
    thread->start(callable_mp(this, &UtilityServer::thread_func));
    return OK;
}

void UtilityServer::finish()
{
    if (!thread)
        return;

    exit_thread = true;
    thread->wait_to_finish();

    memdelete(thread);

    if (mutex)
        memdelete(mutex);

    thread = nullptr;
}


void UtilityServer::lock()
{
    if (!thread || !mutex)
        return;

    mutex->lock();
}

void UtilityServer::unlock()
{
    if (!thread || !mutex)
        return;

    mutex->unlock();
}

UtilityServer::UtilityServer() = default;

UtilityServer::~UtilityServer() = default;
