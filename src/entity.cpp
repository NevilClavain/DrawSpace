#include "entity.h"

using namespace DrawSpace;

int Entity::CheckComponent( ComponentType p_type )
{
    return m_components.count( p_type );
}

void Entity::operator+= (ComponentBase* p_comp)
{
    m_components[p_comp->m_type].push_back( p_comp );
}