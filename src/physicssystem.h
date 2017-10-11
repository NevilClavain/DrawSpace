/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _PHYSICSSYSTEM_H_
#define _PHYSICSSYSTEM_H_

#include "entitynodegraph.h"
#include "timemanager.h"

namespace DrawSpace
{
namespace Systems
{
class PhysicsSystem
{
protected:

    DrawSpace::Utils::TimeManager* m_tm;

public:

    PhysicsSystem( void );
    ~PhysicsSystem( void );

    void Run( EntityGraph::EntityNodeGraph* p_entitygraph );
    void VisitEntity( Core::Entity* p_entity );

    void SetTimeManager( DrawSpace::Utils::TimeManager* p_tm );

};
}
}

#endif