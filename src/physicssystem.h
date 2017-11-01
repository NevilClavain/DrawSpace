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

#include "systems.h"
#include "entitynodegraph.h"
#include "timemanager.h"

namespace DrawSpace
{
namespace Systems
{
class PhysicsSystem : public Interface::System
{
protected:
    
    std::set<Core::Entity*>             m_world_bodies_list; // pour chaque entitee comportant un aspect "Physics", liste des Entites comportant un aspect "Body" trouvees "sous" celle ci (entites filles et sous filles)
    EntityGraph::EntityNodeGraph*       m_current_graph;

public:

    PhysicsSystem( void );
    ~PhysicsSystem( void );

    void Init( void ) {};
    void Release( void ) {};

    void Run( EntityGraph::EntityNodeGraph* p_entitygraph );
    void VisitEntity( Core::Entity* p_parent, Core::Entity* p_entity );
};
}
}

#endif