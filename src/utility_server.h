#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/mutex.hpp>

#include <godot_cpp/templates/rid_owner.hpp>
#include <godot_cpp/templates/rb_map.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>

#include "need.h"

class UtilityServer : public godot::Object
{
    GDCLASS(UtilityServer, godot::Object);

    mutable bool m_exit_thread{false};
    godot::Thread* m_thread{nullptr};
    godot::Mutex* m_mutex{nullptr};

    struct InternalAgent
    {
        godot::Vector<godot::Ref<Need>> needs;
        godot::Vector<float> values;
        godot::RBMap<godot::String, int> indices;

        godot::Callable action_callback;
        godot::Callable no_action_callback;

        float consideration_fraction{0.9};
        float consideration_weight{0.0};
        uint64_t last_decay_tick{0};
    };

    struct InternalAction
    {
        godot::RBMap<godot::String, float> advert;
        bool active{false};
        float spatial_weight{1.0};
    };

    mutable godot::RID_PtrOwner<InternalAgent> m_agents;
    mutable godot::RID_PtrOwner<InternalAction> m_actions;


    static UtilityServer* s_singleton;
    void thread_func();

    InternalAgent* get_agent_with_decays(godot::RID agent);

protected:
    static void _bind_methods();

public:
    static UtilityServer* get_singleton();

    godot::Error init();
    void finish();

    // Allocation
    godot::RID create_agent();
    godot::RID create_action();
    void free_rid(godot::RID rid);

    // Agent settings
    void agent_set_needs(godot::RID agent, const godot::TypedArray<Need>& needs);
    godot::TypedArray<Need> agent_get_needs(godot::RID agent) const;

    UtilityServer();
    ~UtilityServer();
};
