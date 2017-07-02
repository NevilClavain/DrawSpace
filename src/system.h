
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "entityset.h"

namespace DrawSpace
{

class System abstract // est un visiteur de EntitySet
{
protected:

public:
    virtual void VisitEntitySet(Entity* p_entity) = 0;

};

}

#endif