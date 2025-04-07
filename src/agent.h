#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>

#include "need.h"

class Agent : public godot::Node2D
{
    GDCLASS(Agent, godot::Node2D);

    godot::RID m_rid;

    godot::TypedArray<Need> m_needs;
    float m_consideration_fraction{0.9};
    float m_consideration_weight{0.5};
    bool m_decaying{true};
    godot::TypedArray<godot::String> m_tags;

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    void set_needs(const godot::TypedArray<Need>& needs);
    godot::TypedArray<Need> get_needs() const;
    void update_needs();

    void set_consideration_fraction(float fraction);
    float get_consideration_fraction() const;

    void set_consideration_weight(float weight);
    float get_consideration_weight() const;

    void set_decaying(bool decaying);
    bool get_decaying() const;

    void set_tags(const godot::TypedArray<godot::String>& tags);
    godot::TypedArray<godot::String> get_tags() const;

    float get_need_value(const godot::String& name) const;
    void set_need_value(const godot::String& name, float value);

    void choose_action(float near_distance, float far_distance, const godot::TypedArray<godot::String>& tags = {});
    void grant(const godot::TypedDictionary<godot::String, float>& reward);

    void callback_action(uint64_t instance_id);
    void callback_no_action();

    Agent();
    ~Agent();
};
