#include "action.h"

#include "utility_server.h"

using namespace godot;

void Action::_bind_methods()
{
}

Action::Action()
{
    m_rid = UtilityServer::get_singleton()->create_action();
}

Action::~Action()
{
    UtilityServer::get_singleton()->free_rid(m_rid);
}
