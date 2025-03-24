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
    ClassDB::bind_method(D_METHOD("agent_set_action_callback", "rid", "callback"), &UtilityServer::agent_set_action_callback);
    ClassDB::bind_method(D_METHOD("agent_set_no_action_callback", "rid", "callback"), &UtilityServer::agent_set_no_action_callback);
    ClassDB::bind_method(D_METHOD("agent_set_consideration", "rid", "fraction", "weight"), &UtilityServer::agent_set_consideration);
    ClassDB::bind_method(D_METHOD("agent_get_need_score", "rid", "need"), &UtilityServer::agent_get_need_score);
    ClassDB::bind_method(D_METHOD("agent_set_need_score", "rid", "need", "value"), &UtilityServer::agent_set_need_score);

    ClassDB::bind_method(D_METHOD("action_set_active", "rid", "active"), &UtilityServer::action_set_active);
    ClassDB::bind_method(D_METHOD("action_set_advert", "rid", "advert"), &UtilityServer::action_set_advert);
    ClassDB::bind_method(D_METHOD("action_set_spatial_weight", "rid", "spatial_weight"), &UtilityServer::action_set_spatial_weight);
    ClassDB::bind_method(D_METHOD("action_set_object_id", "rid", "instance_id"), &UtilityServer::action_set_object_id);
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

void UtilityServer::agent_set_action_callback(RID agent, Callable callback)
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL(a);

    a->action_callback = callback;
}

void UtilityServer::agent_set_no_action_callback(RID agent, Callable callback)
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL(a);

    a->no_action_callback = callback;
}

void UtilityServer::agent_set_consideration(RID agent, float fraction, float weight)
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL(a);

    a->consideration_fraction = fraction;
    a->consideration_weight = weight;
}

float UtilityServer::agent_get_need_score(RID agent, String need) const
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL_V(a, 0.0f);

    decltype(a->indices)::Element* v = a->indices.find(need);
    ERR_FAIL_NULL_V_MSG(v, 0.0f, "Need not found");
    return a->values[v->value()];
}

void UtilityServer::agent_set_need_score(RID agent, String need, float score)
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL(a);

    decltype(a->indices)::Element* v = a->indices.find(need);
    ERR_FAIL_NULL_MSG(v, "Need not found");
    a->values.write[v->value()] = CLAMP(score, 0.0f, 1.0f);
}

void UtilityServer::action_set_active(RID action, bool active)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->active = active;
}

void UtilityServer::action_set_advert(RID action, const TypedDictionary<String, float>& advert)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->advert.clear();
    Array keys = advert.keys();
    for (int n = 0; n < keys.size(); ++n)
    {
        String key = keys[n];
        float value = advert[key];
        a->advert.insert(key, value);
    }
}

void UtilityServer::action_set_spatial_weight(RID action, float spatial_weight)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->spatial_weight = spatial_weight;
}

void UtilityServer::action_set_object_id(RID action, uint64_t instance_id)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->instance_id = instance_id;
}

UtilityServer::UtilityServer()
{
    s_singleton = this;
}

UtilityServer::~UtilityServer() = default;
