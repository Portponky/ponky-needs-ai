#include "register_types.h"

#include "need.h"
#include "agent.h"
#include "action.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_utility_ai(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;

    GDREGISTER_RUNTIME_CLASS(Need);
    GDREGISTER_RUNTIME_CLASS(Agent);
    GDREGISTER_RUNTIME_CLASS(Action);
}

void uninitialize_utility_ai(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
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

