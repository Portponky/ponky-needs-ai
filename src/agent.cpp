#include "agent.h"

#include "utility_server.h"

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

Agent::Agent()
{
    m_rid = UtilityServer::get_singleton()->create_agent();
    // Add action and no-action callbacks
}

Agent::~Agent()
{
    UtilityServer::get_singleton()->free_rid(m_rid);
}
