#pragma once

#include <godot_cpp/classes/node2d.hpp>

class Agent : public godot::Node2D
{
    GDCLASS(Agent, godot::Node2D);

    godot::RID m_rid;

protected:
    static void _bind_methods();

public:
    Agent();
    ~Agent();
};
