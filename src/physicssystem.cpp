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

#include "physicssystem.h"
#include "physicsaspect.h"
#include "bodyaspect.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Systems;

PhysicsSystem::PhysicsSystem( void ) : 
m_tm( NULL )
{
}

PhysicsSystem::~PhysicsSystem( void )
{
}

void PhysicsSystem::Run( EntityNodeGraph* p_entitygraph )
{
    if( NULL == m_tm )
    {
        _DSEXCEPTION( "no time manager setted for physics system" );
    }

    m_bodies_list.clear(); // clear list of entities with body aspect
    p_entitygraph->AcceptPhysicsSystem( this );
}

void PhysicsSystem::VisitEntity( Entity* p_entity )
{
    PhysicsAspect* physics_aspect = p_entity->GetAspect<PhysicsAspect>();
    if( physics_aspect )
    {
        // submit current Body entities list to physic aspect
        physics_aspect->UpdateBodiesList( m_bodies_list );

        physics_aspect->StepSimulation( m_tm->GetFPS(), 15 );

        m_bodies_list.clear(); // clear list for next entity with Physics aspect (if exists)
    }
    else
    {
        BodyAspect* body_aspect = p_entity->GetAspect<BodyAspect>();
        if( body_aspect )
        {
            m_bodies_list.push_back( p_entity ); // memorize this entity with Body aspect
        }
    }
}

void PhysicsSystem::SetTimeManager( DrawSpace::Utils::TimeManager* p_tm )
{
    m_tm = p_tm;
}