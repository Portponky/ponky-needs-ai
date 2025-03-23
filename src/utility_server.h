#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/mutex.hpp>
//#include "core/templates/list.h"
//#include "core/templates/rid.h"
//#include "core/templates/set.h"
//#include "core/variant/variant.h"

class UtilityServer : public godot::Object
{
    GDCLASS(UtilityServer, godot::Object);

    void thread_func();

    mutable bool exit_thread{false};
    godot::Thread* thread{nullptr};
    godot::Mutex* mutex{nullptr};

protected:
    static void _bind_methods();

public:
    godot::Error init();
    void finish();

    void lock();
    void unlock();

    UtilityServer();
    ~UtilityServer();
};
