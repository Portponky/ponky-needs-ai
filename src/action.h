#pragma once

#include <godot_cpp/classes/node2d.hpp>

#include <godot_cpp/variant/typed_dictionary.hpp>

class Action : public godot::Node2D
{
    GDCLASS(Action, godot::Node2D);

    godot::RID m_rid;

    godot::TypedDictionary<godot::String, float> m_advert;
    bool m_active{true};
    float m_spatial_weight{1.0f};
    godot::TypedArray<godot::String> m_tags;

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    void set_advert(const godot::TypedDictionary<godot::String, float>& advert);
    godot::TypedDictionary<godot::String, float> get_advert() const;
    void update_advert();

    void set_active(bool active);
    bool get_active() const;

    void set_stock(int stock);
    int get_stock() const;

    void set_spatial_weight(float weight);
    float get_spatial_weight() const;

    void set_tags(const godot::TypedArray<godot::String>& tags);
    godot::TypedArray<godot::String> get_tags() const;
    void add_tag(const godot::String& tag);
    void remove_tag(const godot::String& tag);

    Action();
    ~Action();
};
