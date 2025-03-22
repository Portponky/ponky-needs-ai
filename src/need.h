#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/curve.hpp>

class Need : public godot::Resource
{
    GDCLASS(Need, godot::Resource);

    godot::String m_name;
    godot::Ref<godot::Curve> m_response;
    float m_attenuation_weight{1.0f};
    float m_decay_time{0.0f};
    float m_initial_value{0.5f};

protected:
    static void _bind_methods();

public:
    void set_name(const godot::String& name);
    godot::String get_name() const;

    void set_response(const godot::Ref<godot::Curve>& response);
    godot::Ref<godot::Curve> get_response() const;

    void set_attenuation_weight(const float weight);
    float get_attenuation_weight() const;

    void set_decay_time(const float time);
    float get_decay_time() const;

    void set_initial_value(const float value);
    float get_initial_value() const;
};
