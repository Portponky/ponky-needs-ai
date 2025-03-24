#pragma once

#include <godot_cpp/classes/node2d.hpp>

#include "need.h"

class Agent : public godot::Node2D
{
    GDCLASS(Agent, godot::Node2D);

    godot::RID m_rid;

    godot::TypedArray<Need> m_needs;
    float m_consideration_fraction{0.9};
    float m_consideration_weight{0.5};

protected:
    static void _bind_methods();

public:
    void set_needs(const godot::TypedArray<Need>& needs);
    godot::TypedArray<Need> get_needs() const;

    void set_consideration_fraction(float fraction);
    float get_consideration_fraction() const;

    void set_consideration_weight(float weight);
    float get_consideration_weight() const;

    Agent();
    ~Agent();
};
