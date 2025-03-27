#include "agent.h"

#include "utility_server.h"
#include "action.h"

using namespace godot;

void Agent::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_needs", "needs"), &Agent::set_needs);
    ClassDB::bind_method(D_METHOD("get_needs"), &Agent::get_needs);

    ADD_PROPERTY(
        PropertyInfo(
            Variant::ARRAY,
            "needs",
            PROPERTY_HINT_TYPE_STRING,
            String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":Need"
        ),
        "set_needs",
        "get_needs"
    );

    ClassDB::bind_method(D_METHOD("set_consideration_fraction", "fraction"), &Agent::set_consideration_fraction);
    ClassDB::bind_method(D_METHOD("get_consideration_fraction"), &Agent::get_consideration_fraction);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "consideration_fraction", PROPERTY_HINT_RANGE, "0.0,1.0"), "set_consideration_fraction", "get_consideration_fraction");

    ClassDB::bind_method(D_METHOD("set_consideration_weight", "weight"), &Agent::set_consideration_weight);
    ClassDB::bind_method(D_METHOD("get_consideration_weight"), &Agent::get_consideration_weight);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "consideration_weight", PROPERTY_HINT_RANGE, "0.0,1.0"), "set_consideration_weight", "get_consideration_weight");

    ClassDB::bind_method(D_METHOD("set_decaying", "decaying"), &Agent::set_decaying);
    ClassDB::bind_method(D_METHOD("get_decaying"), &Agent::get_decaying);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "decaying"), "set_decaying", "get_decaying");

    ClassDB::bind_method(D_METHOD("set_tags", "tags"), &Agent::set_tags);
    ClassDB::bind_method(D_METHOD("get_tags"), &Agent::get_tags);

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "tags", PROPERTY_HINT_TYPE_STRING, String::num(Variant::STRING) + ":"), "set_tags", "get_tags");

    ClassDB::bind_method(D_METHOD("get_need_value", "name"), &Agent::get_need_value);
    ClassDB::bind_method(D_METHOD("set_need_value", "name", "value"), &Agent::set_need_value);
    ClassDB::bind_method(D_METHOD("choose_action", "near_distance", "far_distance", "tags"), &Agent::choose_action, DEFVAL(TypedArray<String>{}));
    ClassDB::bind_method(D_METHOD("grant", "reward"), &Agent::grant);

    ADD_SIGNAL(MethodInfo("action_chosen", PropertyInfo(Variant::OBJECT, "action", PROPERTY_HINT_RESOURCE_TYPE, "Action")));
    ADD_SIGNAL(MethodInfo("no_action_chosen"));
}

void Agent::_notification(int what)
{
    switch (what)
    {
    case NOTIFICATION_ENTER_TREE:
        UtilityServer::get_singleton()->agent_set_decaying(m_rid, m_decaying);
        break;
    case NOTIFICATION_EXIT_TREE:
        UtilityServer::get_singleton()->agent_set_decaying(m_rid, false);
        break;
    }
}

void Agent::set_needs(const TypedArray<Need>& needs)
{
    m_needs = needs;
    UtilityServer::get_singleton()->agent_set_needs(m_rid, needs);
}

TypedArray<Need> Agent::get_needs() const
{
    return m_needs;
}

void Agent::set_consideration_fraction(float fraction)
{
    m_consideration_fraction = fraction;
    UtilityServer::get_singleton()->agent_set_consideration(m_rid, m_consideration_fraction, m_consideration_weight);
}

float Agent::get_consideration_fraction() const
{
    return m_consideration_fraction;
}

void Agent::set_consideration_weight(float weight)
{
    m_consideration_weight = weight;
    UtilityServer::get_singleton()->agent_set_consideration(m_rid, m_consideration_fraction, m_consideration_weight);
}

float Agent::get_consideration_weight() const
{
    return m_consideration_weight;
}

void Agent::set_decaying(bool decaying)
{
    m_decaying = decaying;
    if (is_inside_tree())
        UtilityServer::get_singleton()->agent_set_decaying(m_rid, m_decaying);
}

bool Agent::get_decaying() const
{
    return m_decaying;
}

void Agent::set_tags(const godot::TypedArray<godot::String>& tags)
{
    m_tags = tags;
}

godot::TypedArray<godot::String> Agent::get_tags() const
{
    return m_tags;
}

float Agent::get_need_value(const godot::String& name) const
{
    return UtilityServer::get_singleton()->agent_get_need_score(m_rid, name);
}

void Agent::set_need_value(const godot::String& name, float value)
{
    UtilityServer::get_singleton()->agent_set_need_score(m_rid, name, value);
}

void Agent::choose_action(float near_distance, float far_distance, const godot::TypedArray<godot::String>& tags)
{
    ERR_FAIL_COND(near_distance < 0.0f);
    ERR_FAIL_COND(far_distance <= near_distance);

    TypedArray<String> all_tags = m_tags;
    all_tags.append_array(m_tags);

    UtilityServer::get_singleton()->agent_choose_action(m_rid, get_global_position(), near_distance, far_distance, all_tags);
}

void Agent::grant(const godot::TypedDictionary<godot::String, float>& reward)
{
    UtilityServer::get_singleton()->agent_grant(m_rid, reward);
}

void Agent::callback_action(uint64_t instance_id)
{
    Object* object = UtilityFunctions::instance_from_id(instance_id);
    if (object)
    {
        Action* action = Object::cast_to<Action>(object);
        if (action)
        {
            emit_signal("action_chosen", action);
            return;
        }
        else
        {
            WARN_PRINT("Action provided object instance id of non-action");
        }
    }
    else
    {
        WARN_PRINT("Failed to find selected action");
    }

    callback_no_action();
}

void Agent::callback_no_action()
{
    emit_signal("no_action_chosen");
}

Agent::Agent()
{
    m_rid = UtilityServer::get_singleton()->create_agent();
    UtilityServer::get_singleton()->agent_set_action_callback(m_rid, callable_mp(this, &Agent::callback_action));
    UtilityServer::get_singleton()->agent_set_no_action_callback(m_rid, callable_mp(this, &Agent::callback_no_action));
}

Agent::~Agent()
{
    UtilityServer::get_singleton()->free_rid(m_rid);
}
