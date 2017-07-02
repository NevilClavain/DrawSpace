#ifndef _ENTITYSET_H_
#define _ENTITYSET_H_

#include "entity.h"
#include "st_tree.h"
#include <vector>

namespace DrawSpace
{
class System;
struct EntitySet
{
private:

    std::vector<st_tree::tree<Entity*>>     m_entities;

public:

    void InsertTree( const st_tree::tree<Entity*>& p_tree );

    void AcceptSystemTopDownRecursive(System* p_system);
};

}

#endif
