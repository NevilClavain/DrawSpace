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

#include "physicssystem.h"
#include "physicsaspect.h"
#include "collisionaspect.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Systems;

PhysicsSystem::PhysicsSystem( void )
{
}

PhysicsSystem::~PhysicsSystem( void )
{
}

void PhysicsSystem::run( EntityNodeGraph* p_entitygraph )
{
    //m_world_bodies_list.clear(); // clear list of entities with body aspect
    m_current_graph = p_entitygraph;
    m_current_graph->AcceptSystemLeafToRoot( this );
}

void PhysicsSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{
    PhysicsAspect* physics_aspect = p_entity->GetAspect<PhysicsAspect>();
    if( physics_aspect )
    {
        // submit current Body entities list to physic aspect

        physics_aspect->StepSimulation();
    }

    CollisionAspect* collision_aspect = p_entity->GetAspect<CollisionAspect>();
    if (collision_aspect)
    {
        collision_aspect->Update(p_entity);
    }
}
