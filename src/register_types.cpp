#include "register_types.h"

#include "need.h"
#include "agent.h"
#include "action.h"
#include "utility_server.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

namespace
{

static UtilityServer* s_utility_server = nullptr;
const char* UTILITY_SERVER_NAME = "UtilityServer";

}

void initialize_utility_ai(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    GDREGISTER_RUNTIME_CLASS(Need);
    GDREGISTER_RUNTIME_CLASS(Agent);
    GDREGISTER_RUNTIME_CLASS(Action);

    GDREGISTER_ABSTRACT_CLASS(UtilityServer);

    s_utility_server = memnew(UtilityServer);
    s_utility_server->init();
    godot::Engine::get_singleton()->register_singleton(UTILITY_SERVER_NAME, s_utility_server);
}

void uninitialize_utility_ai(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    if (s_utility_server)
    {
        godot::Engine::get_singleton()->unregister_singleton(UTILITY_SERVER_NAME);
        s_utility_server->finish();
        memdelete(s_utility_server);
        s_utility_server = nullptr;
    }
}

extern "C" {

GDExtensionBool GDE_EXPORT utility_ai_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
{
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_utility_ai);
	init_obj.register_terminator(uninitialize_utility_ai);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}

}

