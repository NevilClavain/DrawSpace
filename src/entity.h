#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <set>
#include "component.h"

namespace DrawSpace
{
    typedef std::set<ComponentBase*> Entity;
}

#endif