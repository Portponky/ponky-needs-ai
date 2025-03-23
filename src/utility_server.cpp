#include "utility_server.h"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

UtilityServer* UtilityServer::s_singleton{nullptr};

void UtilityServer::thread_func()
{
    uint64_t msdelay = 1000;

    while (!m_exit_thread)
    {
        OS::get_singleton()->delay_usec(msdelay * 1000);
    }
}

void UtilityServer::_bind_methods()
{

}

UtilityServer* UtilityServer::get_singleton()
{
    return s_singleton;
}

Error UtilityServer::init()
{
    m_exit_thread = false;
    m_mutex = memnew(Mutex);
    m_thread = memnew(Thread);
    m_thread->reference(); // Fudge: The thread seems to get unreffed an extra time whilst starting
    m_thread->start(callable_mp(this, &UtilityServer::thread_func));
    return OK;
}

void UtilityServer::finish()
{
    if (!m_thread)
        return;

    m_exit_thread = true;
    m_thread->wait_to_finish();

    memdelete(m_thread);

    if (m_mutex)
        memdelete(m_mutex);

    m_thread = nullptr;
}

RID UtilityServer::create_agent()
{
    return m_agents.make_rid(InternalAgent());
}

void UtilityServer::free_rid(godot::RID rid)
{
    if (m_agents.owns(rid))
    {
        m_agents.free(rid);
    }
    else
        ERR_FAIL_MSG("Invalid ID.");
}

UtilityServer::UtilityServer()
{
    s_singleton = this;
}

UtilityServer::~UtilityServer() = default;
