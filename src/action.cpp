#include "action.h"

#include "utility_server.h"

using namespace godot;

void Action::_bind_methods()
{
    // advert as property

    ClassDB::bind_method(D_METHOD("set_active", "active"), &Action::set_active);
    ClassDB::bind_method(D_METHOD("get_active"), &Action::get_active);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "active"), "set_active", "get_active");

    ClassDB::bind_method(D_METHOD("set_spatial_weight", "weight"), &Action::set_spatial_weight);
    ClassDB::bind_method(D_METHOD("get_spatial_weight"), &Action::get_spatial_weight);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "spatial_weight", PROPERTY_HINT_RANGE, "0.0,2.0,or_greater"), "set_spatial_weight", "get_spatial_weight");
}

void Action::_notification(int what)
{
    switch (what)
    {
    case NOTIFICATION_ENTER_TREE:
        UtilityServer::get_singleton()->action_set_active(m_rid, m_active);
        break;
    case NOTIFICATION_EXIT_TREE:
        UtilityServer::get_singleton()->action_set_active(m_rid, false);
        break;
    }
}

void Action::set_advert(const godot::TypedDictionary<godot::String, float>& advert)
{
    m_advert = advert;
    UtilityServer::get_singleton()->action_set_advert(m_rid, m_advert);
}

godot::TypedDictionary<godot::String, float> Action::get_advert() const
{
    return m_advert;
}

void Action::set_active(bool active)
{
    m_active = active;
    if (is_inside_tree())
        UtilityServer::get_singleton()->action_set_active(m_rid, m_active);
}

bool Action::get_active() const
{
    return m_active;
}

void Action::set_spatial_weight(float weight)
{
    m_spatial_weight = weight;
    UtilityServer::get_singleton()->action_set_spatial_weight(m_rid, m_spatial_weight);
}

float Action::get_spatial_weight() const
{
    return m_spatial_weight;
}

Action::Action()
{
    m_rid = UtilityServer::get_singleton()->create_action();
    // add object instance id as callback source
}

Action::~Action()
{
    UtilityServer::get_singleton()->free_rid(m_rid);
}
