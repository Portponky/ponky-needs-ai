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

UtilityServer::InternalAgent* UtilityServer::get_agent_with_decays(RID agent)
{
    // **************** NOT DONE *******************
    return m_agents.get_or_null(agent);
}

void UtilityServer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("create_agent"), &UtilityServer::create_agent);
    ClassDB::bind_method(D_METHOD("create_action"), &UtilityServer::create_action);
    ClassDB::bind_method(D_METHOD("free_rid", "rid"), &UtilityServer::free_rid);

    ClassDB::bind_method(D_METHOD("agent_set_needs", "rid", "needs"), &UtilityServer::agent_set_needs);
    ClassDB::bind_method(D_METHOD("agent_get_needs", "rid"), &UtilityServer::agent_get_needs);
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
    InternalAgent* agent = memnew(InternalAgent);
    RID rid = m_agents.make_rid(agent);
    return rid;
}

RID UtilityServer::create_action()
{
    InternalAction* action = memnew(InternalAction);
    RID rid = m_actions.make_rid(action);
    return rid;
}

void UtilityServer::free_rid(RID rid)
{
    if (m_agents.owns(rid))
    {
        InternalAgent* agent = m_agents.get_or_null(rid);
        m_agents.free(rid);
        memdelete(agent);
    }
    else if (m_actions.owns(rid))
    {
        InternalAction* action = m_actions.get_or_null(rid);
        m_actions.free(rid);
        memdelete(action);
    }
    else
        ERR_FAIL_MSG("Invalid ID.");
}

void UtilityServer::agent_set_needs(RID agent, const TypedArray<Need>& needs)
{
    InternalAgent* a = get_agent_with_decays(agent);
    ERR_FAIL_NULL(a);

    RBMap<String, float> prev_values;
    for (const auto& it : a->indices)
        prev_values[it.key] = a->values[it.value];

    a->needs.clear();
    a->values.clear();
    a->indices.clear();

    for (int n = 0; n < needs.size(); ++n)
    {
        Ref<Need> need = needs[n];
        if (a->indices.find(need->get_name()))
        {
            WARN_PRINT("Duplicate need name found");
            continue;
        }

        a->indices[need->get_name()] = a->needs.size();
        a->needs.push_back(need);

        decltype(prev_values)::Element* v = prev_values.find(need->get_name());
        a->values.push_back(v ? v->value() : need->get_initial_value());
    }
}

godot::TypedArray<Need> UtilityServer::agent_get_needs(RID agent) const
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL_V(a, {});

    TypedArray<Need> needs;
    for (const Ref<Need>& need : a->needs)
        needs.push_back(need);
    return needs;
}

UtilityServer::UtilityServer()
{
    s_singleton = this;
}

UtilityServer::~UtilityServer() = default;
