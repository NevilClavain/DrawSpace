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

#include "physicsaspect.h"
#include "vector.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;

PhysicsAspect::PhysicsAspect( void ) :
m_collisionDispatcher( &m_collisionConfiguration ),
m_world( &m_collisionDispatcher, &m_broadphase, &m_sequentialImpulseConstraintSolver, &m_collisionConfiguration),
m_gravity_applied( false )
{
    m_world.setGravity( btVector3( 0.0, 0.0, 0.0 ) );
}

void PhysicsAspect::on_added_bodyentity( Entity* p_entity )
{
    _asm nop
}

void PhysicsAspect::on_removed_bodyentity( Entity* p_entity )
{
    _asm nop
}

void PhysicsAspect::UpdateBodiesList( const std::set<Entity*>& p_list )
{
    
    for( auto& it = p_list.begin(); it != p_list.end(); ++it )
    {
        Entity* curr_entity = *it; 

        if( 0 == m_bodies_set.count( curr_entity ) )
        {
            on_added_bodyentity( curr_entity );
            m_bodies_set.insert( curr_entity );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////

    std::vector<Entity*> to_remove;
    for( auto& it = m_bodies_set.begin(); it != m_bodies_set.end(); ++it )
    {
        //chercher dans p_list... (passer plutot un set dans p_list ?)
        Entity* curr_entity = *it;

        if( 0 == p_list.count( curr_entity ) )
        {
            // cette entité n'est pas dans la liste fournie en entrée -> a été retirée du graph
            on_removed_bodyentity( curr_entity );
            to_remove.push_back( curr_entity );
        }
    }

    for( size_t i = 0; i < to_remove.size(); ++i )
    {
        // effacer de m_bodies_set
        m_bodies_set.erase( to_remove[i] );

        // effacer de m_bodies
        for( auto& it = m_bodies.begin(); it != m_bodies.end(); ++it )
        {
            if( (*it).second == to_remove[i] )
            {
                m_bodies.erase( it );
                break;
            }
        }
    }
}

void PhysicsAspect::StepSimulation( dsreal p_fps, int p_nbsteps )
{

    ComponentList<bool> flags;
    GetComponentsByType<bool>( flags );

    bool enable_gravity = false;
    Vector gravity;

    if( flags.size() > 0 && true == flags[0]->getPurpose() )
    {
        enable_gravity = true;
    }

    if( enable_gravity )
    {
        ComponentList<Vector> vecs;
        GetComponentsByType<Vector>( vecs );

        gravity = vecs[0]->getPurpose();
    }

    if( m_gravity_applied != enable_gravity )
    {
        m_gravity_applied = enable_gravity;

        if( m_gravity_applied )
        {
            m_world.setGravity( btVector3( gravity[0], gravity[1], gravity[2] ) );
        }
        else
        {
            m_world.setGravity( btVector3( 0.0, 0.0, 0.0 ) );
        }
    }


    /*
    // doing timestep < fixedtimestep * 5 (see http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World )
        
    dsreal timestep = 1.0 / p_fps;
    dsreal fixedtimestep = timestep / 5.0;
    fixedtimestep *= 1.1;

    m_world.stepSimulation( timestep, 5 );
    */

    m_world.stepSimulation( 1.0 / p_fps, p_nbsteps );

    // check for collisions

    // provisoire
    /*
    for( std::map<btRigidBody*, Body*>::iterator it = m_bodies.begin(); it != m_bodies.end(); ++it )
    {
        (*it).second->SetContactState( false );
    }
    */

    int numManifolds = m_world.getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; i++ )
	{
		btPersistentManifold* contactManifold =  m_world.getDispatcher()->getManifoldByIndexInternal(i);

        //btRigidBody* obA = static_cast<btRigidBody*>( contactManifold->getBody0() );

        btRigidBody* obA = (btRigidBody*)( contactManifold->getBody0() );
        btRigidBody* obB = (btRigidBody*)( contactManifold->getBody1() );

        int numContacts = contactManifold->getNumContacts();

        // provisoire
        /*
        if( m_bodies.count( obA ) > 0 )
        {
            if( numContacts > 0 )
            {
                m_bodies[obA]->SetContactState( true );
            }
        }
        else if( m_bodies.count( obB ) > 0 )
        {
            if( numContacts > 0 )
            {
                m_bodies[obB]->SetContactState( true );
            }
        }
        */
	}
}