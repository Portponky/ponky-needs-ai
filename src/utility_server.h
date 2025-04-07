#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/semaphore.hpp>

#include <godot_cpp/templates/rid_owner.hpp>
#include <godot_cpp/templates/rb_map.hpp>
#include <godot_cpp/templates/rb_set.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>

#include "need.h"

class UtilityServer : public godot::Object
{
    GDCLASS(UtilityServer, godot::Object);

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
        bool decaying{false};
    };

    struct InternalAction
    {
        godot::RBMap<godot::String, float> advert;
        bool active{false};
        float spatial_weight{1.0};
        godot::Vector2 position;
        godot::ObjectID instance_id;
        godot::Vector<godot::String> tags_yes;
        godot::Vector<godot::String> tags_no;
    };

    struct ThinkRequest
    {
        godot::RID agent;
        godot::Vector2 position;
        float near_range;
        float far_range;
        godot::RBSet<godot::String> tags;
    };

    mutable bool m_exit_thread{false};
    godot::Ref<godot::Thread> m_thread{nullptr};

    mutable godot::RID_PtrOwner<InternalAgent> m_agents;
    mutable godot::RID_PtrOwner<InternalAction> m_actions;

    godot::Mutex* m_input_mutex{nullptr};
    godot::Semaphore* m_work_semaphore{nullptr};
    godot::Vector<ThinkRequest> m_requests;
    godot::Vector<godot::RID> m_free_queue;
    bool m_work_complete{true};

    bool m_initialized_process_callback{false};

    uint64_t m_last_step_time{0};

    static UtilityServer* s_singleton;
    void thread_func();
    void purge_free_queue();
    bool filter(const ThinkRequest& t, InternalAction* action) const;
    void think(const ThinkRequest& t);

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
    void agent_set_action_callback(godot::RID agent, godot::Callable callback);
    void agent_set_no_action_callback(godot::RID agent, godot::Callable callback);
    void agent_set_consideration(godot::RID agent, float fraction, float weight);
    void agent_set_decaying(godot::RID agent, bool decaying);
    float agent_get_need_score(godot::RID agent, const godot::String& need);
    void agent_set_need_score(godot::RID agent, const godot::String& need, float score);

    // Action settings
    void action_set_active(godot::RID action, bool active);
    void action_set_advert(godot::RID action, const godot::TypedDictionary<godot::String, float>& advert);
    void action_set_spatial_weight(godot::RID action, float spatial_weight);
    void action_set_position(godot::RID action, const godot::Vector2& position);
    void action_set_object_id(godot::RID action, uint64_t instance_id);
    void action_set_tags(godot::RID action, const godot::TypedArray<godot::String>& tags);

    // Use
    void agent_choose_action(godot::RID agent, godot::Vector2 position, float near_distance, float far_distance, const godot::TypedArray<godot::String>& tags);
    void agent_grant(godot::RID agent, const godot::TypedDictionary<godot::String, float>& reward);

    void step();

    UtilityServer();
    ~UtilityServer();
};
