/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/
/* -*-LIC_END-*- */

#ifndef _TIMESYSTEM_H_
#define _TIMESYSTEM_H_

#include "systems.h"
#include "timemanager.h"
#include "timeaspect.h"

namespace DrawSpace
{
namespace Systems
{
class TimeSystem : public Interface::System
{
protected:
    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

    Aspect::TimeAspect* m_time_aspect;

public:
    TimeSystem( void );
    ~TimeSystem( void );

    void VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity );

    dsstring GetSystemId(void) const { return "TimeSystem"; };
};

}
}

#endif
