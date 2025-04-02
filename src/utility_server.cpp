#include "utility_server.h"

#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

UtilityServer* UtilityServer::s_singleton{nullptr};

void UtilityServer::thread_func()
{
    uint64_t sleep_delay_ms = 20;

    while (!m_exit_thread)
    {
        purge_free_queue();

        bool pending_think{false};
        ThinkRequest next;

        m_input_mutex->lock();
        if (m_requests.size() > 0)
        {
            next = m_requests.get(0);
            m_requests.remove_at(0);
            pending_think = true;
        }
        m_input_mutex->unlock();

        if (pending_think)
            think(next);

        OS::get_singleton()->delay_usec(sleep_delay_ms * 1000);
    }
}

void UtilityServer::purge_free_queue()
{
    m_input_mutex->lock();
    auto free_queue = m_free_queue;
    m_free_queue.clear();
    m_input_mutex->unlock();

    for (RID rid : free_queue)
    {
        if (m_agents.owns(rid))
        {
            InternalAgent* agent = m_agents.get_or_null(rid);
            m_agents.free(rid);
            memdelete(agent);
        }
        else if (m_actions.owns(rid))
        {
            InternalAction* action = m_actions.get_or_null(rid);
            m_actions.free(rid);
            memdelete(action);
        }
    }
}

bool UtilityServer::filter(const ThinkRequest& t, InternalAction* action) const
{
    for (int n = 0; n < action->tags_yes.size(); ++n)
        if (!t.tags.has(action->tags_yes[n]))
            return true;

    for (int n = 0; n < action->tags_no.size(); ++n)
        if (t.tags.has(action->tags_no[n]))
            return true;

    return false;
}

void UtilityServer::think(const ThinkRequest& t)
{
    InternalAgent* ag = get_agent_with_decays(t.agent);
    if (!ag)
        return; // already deleted

    Vector<ObjectID> choices;
    Vector<float> scores;
    float best{0.0f};
    float worst{0.0f};

    List<RID> list;
    m_actions.get_owned_list(&list);
    for (RID rid : list)
    {
        InternalAction* ac = m_actions.get_or_null(rid);
        if (!ac || !ac->active)
            continue; // already deleted

        // filters
        if (filter(t, ac))
            continue;

        // scale for distance
        float dist_scale{1.0f};
        if (ac->spatial_weight > 0.0f)
        {
            float dist = t.position.distance_to(ac->position);
            dist_scale = UtilityFunctions::minf(1.0f, UtilityFunctions::inverse_lerp(t.far_range, t.near_range, ac->spatial_weight * dist));
        }

        if (dist_scale <= 0.0f)
            continue;

        float score{0.0f};
        for (const auto& it : ac->advert)
        {
            decltype(ag->indices)::Element* e = ag->indices.find(it.key);
            if (!e)
                continue;
            Ref<Need> need = ag->needs[e->value()];
            const float value = ag->values[e->value()];
            const float delta = UtilityFunctions::clampf(value + it.value, 0.0, 1.0);
            score += need->get_attenuation_weight() * (need->get_response()->sample(delta) - need->get_response()->sample(value));
        }
        score *= dist_scale;

        if (score > worst)
        {
            choices.append(ac->instance_id);
            scores.push_back(score);

            if (score > best)
            {
                best = score;
                worst = ag->consideration_fraction * best;
            }
        }
    }

    Vector<float> weights;
    float total_weight{0.0f};
    for (int n = 0; n < choices.size(); ++n)
    {
        if (scores[n] <= worst)
            weights.append(0.0f);
        else
        {
            float weight = UtilityFunctions::remap(scores[n], worst, best, ag->consideration_weight, 1.0);
            total_weight += weight;
            weights.push_back(weight);
        }
    }

    if (choices.is_empty() || total_weight == 0.0f)
    {
        ag->no_action_callback.call_deferred();
        return;
    }

    total_weight *= UtilityFunctions::randf();
    ObjectID chosen = choices[choices.size() - 1]; // default last choice
    for (int n = 0; n < choices.size() - 1; ++n)
    {
        total_weight -= weights[n];
        if (total_weight <= 0.0f)
        {
            chosen = choices[n];
            break;
        }
    }

    ag->action_callback.call_deferred(chosen);
}

UtilityServer::InternalAgent* UtilityServer::get_agent_with_decays(RID agent)
{
    InternalAgent* a = m_agents.get_or_null(agent);

    if (a && a->decaying)
    {
        const uint64_t ticks = Time::get_singleton()->get_ticks_msec();
        const uint64_t diff = ticks - a->last_decay_tick;
        constexpr uint64_t MINIMUM_DECAY_THRESHOLD = 100;
        if (diff < MINIMUM_DECAY_THRESHOLD)
            return a;
        const float decay_seconds = 0.001f * diff;
        for (int n = 0; n < a->needs.size(); ++n)
        {
            if (a->needs[n]->get_decay_time() == 0.0)
                continue;

            a->values.write[n] = UtilityFunctions::clampf(a->values[n] - decay_seconds / a->needs[n]->get_decay_time(), 0.0f, 1.0f);
        }
        a->last_decay_tick = ticks;
    }

    return a;
}

void UtilityServer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("create_agent"), &UtilityServer::create_agent);
    ClassDB::bind_method(D_METHOD("create_action"), &UtilityServer::create_action);
    ClassDB::bind_method(D_METHOD("free_rid", "rid"), &UtilityServer::free_rid);

    ClassDB::bind_method(D_METHOD("agent_set_needs", "rid", "needs"), &UtilityServer::agent_set_needs);
    ClassDB::bind_method(D_METHOD("agent_get_needs", "rid"), &UtilityServer::agent_get_needs);
    ClassDB::bind_method(D_METHOD("agent_set_action_callback", "rid", "callback"), &UtilityServer::agent_set_action_callback);
    ClassDB::bind_method(D_METHOD("agent_set_no_action_callback", "rid", "callback"), &UtilityServer::agent_set_no_action_callback);
    ClassDB::bind_method(D_METHOD("agent_set_consideration", "rid", "fraction", "weight"), &UtilityServer::agent_set_consideration);
    ClassDB::bind_method(D_METHOD("agent_set_decaying", "rid", "decaying"), &UtilityServer::agent_set_decaying);
    ClassDB::bind_method(D_METHOD("agent_get_need_score", "rid", "need"), &UtilityServer::agent_get_need_score);
    ClassDB::bind_method(D_METHOD("agent_set_need_score", "rid", "need", "value"), &UtilityServer::agent_set_need_score);

    ClassDB::bind_method(D_METHOD("action_set_active", "rid", "active"), &UtilityServer::action_set_active);
    ClassDB::bind_method(D_METHOD("action_set_advert", "rid", "advert"), &UtilityServer::action_set_advert);
    ClassDB::bind_method(D_METHOD("action_set_spatial_weight", "rid", "spatial_weight"), &UtilityServer::action_set_spatial_weight);
    ClassDB::bind_method(D_METHOD("action_set_object_id", "rid", "instance_id"), &UtilityServer::action_set_object_id);
    ClassDB::bind_method(D_METHOD("action_set_tags", "rid", "tags"), &UtilityServer::action_set_tags);

    ClassDB::bind_method(D_METHOD("agent_choose_action", "rid", "position", "near_distance", "far_distance", "tags"), &UtilityServer::agent_choose_action);
    ClassDB::bind_method(D_METHOD("agent_grant", "rid", "reward"), &UtilityServer::action_set_object_id);
}

UtilityServer* UtilityServer::get_singleton()
{
    return s_singleton;
}

Error UtilityServer::init()
{
    m_exit_thread = false;
    m_input_mutex = memnew(Mutex);
    m_thread = memnew(Thread);
    m_thread->reference(); // Fudge: The thread seems to get unreffed an extra time whilst starting
    m_thread->start(callable_mp(this, &UtilityServer::thread_func));
    return OK;
}

void UtilityServer::finish()
{
    if (!m_thread)
        return;

    m_exit_thread = true;
    m_thread->wait_to_finish();

    memdelete(m_thread);

    if (m_input_mutex)
        memdelete(m_input_mutex);

    m_input_mutex = nullptr;
    m_thread = nullptr;
}

RID UtilityServer::create_agent()
{
    InternalAgent* agent = memnew(InternalAgent);
    RID rid = m_agents.make_rid(agent);
    return rid;
}

RID UtilityServer::create_action()
{
    InternalAction* action = memnew(InternalAction);
    RID rid = m_actions.make_rid(action);
    return rid;
}

void UtilityServer::free_rid(RID rid)
{
    if (m_agents.owns(rid) || m_actions.owns(rid))
    {
        m_input_mutex->lock();
        m_free_queue.append(rid);
        m_input_mutex->unlock();
    }
    else
        ERR_FAIL_MSG("Invalid ID.");
}

void UtilityServer::agent_set_needs(RID agent, const TypedArray<Need>& needs)
{
    InternalAgent* a = get_agent_with_decays(agent);
    ERR_FAIL_NULL(a);

    RBMap<String, float> prev_values;
    for (const auto& it : a->indices)
        prev_values[it.key] = a->values[it.value];

    a->needs.clear();
    a->values.clear();
    a->indices.clear();

    for (int n = 0; n < needs.size(); ++n)
    {
        Ref<Need> need = needs[n];
        if (a->indices.find(need->get_name()))
        {
            WARN_PRINT("Duplicate need name found");
            continue;
        }

        a->indices[need->get_name()] = a->needs.size();
        a->needs.push_back(need);

        decltype(prev_values)::Element* v = prev_values.find(need->get_name());
        a->values.push_back(v ? v->value() : need->get_initial_value());
    }
}

TypedArray<Need> UtilityServer::agent_get_needs(RID agent) const
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL_V(a, {});

    TypedArray<Need> needs;
    for (const Ref<Need>& need : a->needs)
        needs.push_back(need);
    return needs;
}

void UtilityServer::agent_set_action_callback(RID agent, Callable callback)
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL(a);

    a->action_callback = callback;
}

void UtilityServer::agent_set_no_action_callback(RID agent, Callable callback)
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL(a);

    a->no_action_callback = callback;
}

void UtilityServer::agent_set_consideration(RID agent, float fraction, float weight)
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL(a);

    a->consideration_fraction = fraction;
    a->consideration_weight = weight;
}

void UtilityServer::agent_set_decaying(godot::RID agent, bool decaying)
{
    InternalAgent* a = get_agent_with_decays(agent);
    ERR_FAIL_NULL(a);

    a->decaying = decaying;
    a->last_decay_tick = Time::get_singleton()->get_ticks_msec();
}

float UtilityServer::agent_get_need_score(RID agent, const String& need)
{
    InternalAgent* a = get_agent_with_decays(agent);
    ERR_FAIL_NULL_V(a, 0.0f);

    decltype(a->indices)::Element* v = a->indices.find(need);
    ERR_FAIL_NULL_V_MSG(v, 0.0f, "Need not found");
    return a->values[v->value()];
}

void UtilityServer::agent_set_need_score(RID agent, const String& need, float score)
{
    InternalAgent* a = m_agents.get_or_null(agent);
    ERR_FAIL_NULL(a);

    decltype(a->indices)::Element* v = a->indices.find(need);
    ERR_FAIL_NULL_MSG(v, "Need not found");
    a->values.write[v->value()] = CLAMP(score, 0.0f, 1.0f);
}

void UtilityServer::action_set_active(RID action, bool active)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->active = active;
}

void UtilityServer::action_set_advert(RID action, const TypedDictionary<String, float>& advert)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->advert.clear();
    Array keys = advert.keys();
    for (int n = 0; n < keys.size(); ++n)
    {
        String key = keys[n];
        float value = advert[key];
        a->advert.insert(key, value);
    }
}

void UtilityServer::action_set_spatial_weight(RID action, float spatial_weight)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->spatial_weight = spatial_weight;
}

void UtilityServer::action_set_position(godot::RID action, const godot::Vector2& position)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->position = position;
}

void UtilityServer::action_set_object_id(RID action, uint64_t instance_id)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->instance_id = instance_id;
}

void UtilityServer::action_set_tags(godot::RID action, const godot::TypedArray<godot::String>& tags)
{
    InternalAction* a = m_actions.get_or_null(action);
    ERR_FAIL_NULL(a);

    a->tags_yes.clear();
    a->tags_no.clear();
    for (int n = 0; n < tags.size(); ++n)
    {
        godot::String tag = tags[n];
        if (tag .begins_with("-"))
            a->tags_no.push_back(tag.right(-1));
        else
            a->tags_yes.push_back(tag);
    }
}

void UtilityServer::agent_choose_action(godot::RID agent, godot::Vector2 position, float near_distance, float far_distance, const godot::TypedArray<godot::String>& tags)
{
    RBSet<String> tag_set;
    for (int n = 0; n < tags.size(); ++n)
        tag_set.insert(tags[n]);

    m_input_mutex->lock();
    m_requests.append({agent, position, near_distance, far_distance, tag_set});
    m_input_mutex->unlock();
}

void UtilityServer::agent_grant(godot::RID agent, const godot::TypedDictionary<godot::String, float>& reward)
{
    InternalAgent* a = get_agent_with_decays(agent);
    ERR_FAIL_NULL(a);

    Array keys = reward.keys();
    for (int n = 0; n < keys.size(); ++n)
    {
        String key = keys[n];
        decltype(a->indices)::Element* e = a->indices.find(key);
        if (e)
        {
            float diff = reward[key];
            a->values.write[e->value()] = UtilityFunctions::clampf(a->values[e->value()] + diff, 0.0f, 1.0f);
        }
    }
}

UtilityServer::UtilityServer()
{
    s_singleton = this;
}

UtilityServer::~UtilityServer() = default;
