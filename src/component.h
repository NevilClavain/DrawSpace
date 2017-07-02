#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "componenttype.h"

namespace DrawSpace
{
    class ComponentBase
	{
    public:
        ComponentType m_type = DrawSpace::invalidComponentType;
	};

}

#endif