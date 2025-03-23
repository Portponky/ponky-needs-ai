#include "agent.h"

#include "utility_server.h"

using namespace godot;

void Agent::_bind_methods()
{
}

Agent::Agent()
{
    m_rid = UtilityServer::get_singleton()->create_agent();
}

Agent::~Agent()
{
    UtilityServer::get_singleton()->free_rid(m_rid);
}
