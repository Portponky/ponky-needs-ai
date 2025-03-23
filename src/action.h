#pragma once

#include <godot_cpp/classes/node2d.hpp>

class Action : public godot::Node2D
{
    GDCLASS(Action, godot::Node2D);

    godot::RID m_rid;

protected:
    static void _bind_methods();

public:
    Action();
    ~Action();
};
