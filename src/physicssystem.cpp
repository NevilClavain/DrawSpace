/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
#include "bodyaspect.h"
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
    m_world_bodies_list.clear(); // clear list of entities with body aspect
    m_current_graph = p_entitygraph;
    m_current_graph->AcceptSystemLeafToRoot( this );
}

void PhysicsSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{
    PhysicsAspect* physics_aspect = p_entity->GetAspect<PhysicsAspect>();
    if( physics_aspect )
    {
        // submit current Body entities list to physic aspect
        physics_aspect->UpdateBodiesList( m_world_bodies_list );

        physics_aspect->StepSimulation();

        m_world_bodies_list.clear(); // clear list for next entity with Physics aspect (if exists)
    }
    else
    {
        BodyAspect* body_aspect = p_entity->GetAspect<BodyAspect>();
        if( body_aspect )
        {
            m_world_bodies_list.insert( p_entity ); // memorize this entity with Body aspect

            //////////////////////////////////////////////////////////////////////////////////////////

            // recup liste de tout les noeuds entity ancetre de cet entity
            std::vector<Core::Entity*> ancestors;
            m_current_graph->GetEntityAncestorsList( p_entity, ancestors );

            body_aspect->SetAncestorsList( ancestors );

            BodyAspect* attached_to = NULL;

            for( auto it = ancestors.begin(); it != ancestors.end(); ++it )
            {
                Entity* entity_ancestor = *it;

                BodyAspect* body_candidate = entity_ancestor->GetAspect<BodyAspect>();
                if( body_candidate )
                {
                    attached_to = body_candidate;
                    break;
                }
            }
            
            body_aspect->ManageAttachment( attached_to );

            //////////////////////////////////////////////////////////////////////////////////////////

            body_aspect->Update();
        }
    }
}
