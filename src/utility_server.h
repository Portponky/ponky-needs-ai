#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/mutex.hpp>

#include <godot_cpp/templates/rid_owner.hpp>
//#include "core/templates/list.h"
//#include "core/templates/rid.h"
//#include "core/templates/set.h"
//#include "core/variant/variant.h"

class UtilityServer : public godot::Object
{
    GDCLASS(UtilityServer, godot::Object);

    mutable bool m_exit_thread{false};
    godot::Thread* m_thread{nullptr};
    godot::Mutex* m_mutex{nullptr};

    struct InternalAgent
    {
        //var needs: Array[Need]
        //var values: Array[float]
        //var indices: Dictionary[String, int]

        //var action_callback: Callable
        //var no_action_callback: Callable

        float consideration_fraction{0.9};
        float consideration_weight{0.0};
        uint64_t last_decay_tick{0};
    };

    godot::RID_Owner<InternalAgent> m_agents;

    static UtilityServer* s_singleton;
    void thread_func();

protected:
    static void _bind_methods();

public:
    static UtilityServer* get_singleton();

    godot::Error init();
    void finish();

    godot::RID create_agent();
    void free_rid(godot::RID rid);

    UtilityServer();
    ~UtilityServer();
};
