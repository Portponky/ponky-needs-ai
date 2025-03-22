#include "need.h"

using namespace godot;

void Need::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_name", "p_name"), &Need::set_name);
    ClassDB::bind_method(D_METHOD("get_name"), &Need::get_name);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");

    ClassDB::bind_method(D_METHOD("set_response", "p_curve"), &Need::set_response);
    ClassDB::bind_method(D_METHOD("get_response"), &Need::get_response);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "response", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_response", "get_response");

    ClassDB::bind_method(D_METHOD("set_attenuation_weight", "p_weight"), &Need::set_attenuation_weight);
    ClassDB::bind_method(D_METHOD("get_attenuation_weight"), &Need::get_attenuation_weight);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attenuation_weight", PROPERTY_HINT_RANGE, "0.0,2.0,or_greater"), "set_attenuation_weight", "get_attenuation_weight");

    ClassDB::bind_method(D_METHOD("set_decay_time", "p_time"), &Need::set_decay_time);
    ClassDB::bind_method(D_METHOD("get_decay_time"), &Need::get_decay_time);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "decay_time", PROPERTY_HINT_RANGE, "0.0,600.0,or_greater"), "set_decay_time", "get_decay_time");

    ClassDB::bind_method(D_METHOD("set_initial_value", "p_value"), &Need::set_initial_value);
    ClassDB::bind_method(D_METHOD("get_initial_value"), &Need::get_initial_value);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "initial_value", PROPERTY_HINT_RANGE, "0.0,1.0"), "set_initial_value", "get_initial_value");
}

void Need::set_name(const String& name)
{
    m_name = name;
}

String Need::get_name() const
{
    return m_name;
}

void Need::set_response(const Ref<Curve>& response)
{
    m_response = response;
}

Ref<Curve> Need::get_response() const
{
    return m_response;
}

void Need::set_attenuation_weight(const float weight)
{
    m_attenuation_weight = weight;
}

float Need::get_attenuation_weight() const
{
    return m_attenuation_weight;
}

void Need::set_decay_time(const float time)
{
    m_decay_time = time;
}

float Need::get_decay_time() const
{
    return m_decay_time;
}

void Need::set_initial_value(const float value)
{
    m_initial_value = value;
}

float Need::get_initial_value() const
{
    return m_initial_value;
}
