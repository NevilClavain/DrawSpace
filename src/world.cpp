/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "world.h"
#include "body.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;

dsreal World::m_scale = 1.0;

World::World( void ) : 
m_world( NULL )
{
    m_collisionConfiguration            = _DRAWSPACE_NEW_( btDefaultCollisionConfiguration, btDefaultCollisionConfiguration );
    m_collisionDispatcher               = _DRAWSPACE_NEW_( btCollisionDispatcher, btCollisionDispatcher( m_collisionConfiguration ) );
    m_broadphase                        = _DRAWSPACE_NEW_( btDbvtBroadphase, btDbvtBroadphase );
    m_sequentialImpulseConstraintSolver = _DRAWSPACE_NEW_( btSequentialImpulseConstraintSolver, btSequentialImpulseConstraintSolver );
}

World::~World( void )
{
    if( m_world )
    {
        _DRAWSPACE_DELETE_( m_world );
    }

    _DRAWSPACE_DELETE_( m_collisionConfiguration );
    _DRAWSPACE_DELETE_( m_collisionDispatcher );
    _DRAWSPACE_DELETE_( m_broadphase );
    _DRAWSPACE_DELETE_( m_sequentialImpulseConstraintSolver );
}

bool World::Initialize( void )
{
    m_world = _DRAWSPACE_NEW_( btDiscreteDynamicsWorld, btDiscreteDynamicsWorld( m_collisionDispatcher, m_broadphase, m_sequentialImpulseConstraintSolver, m_collisionConfiguration ) );
    m_world->setGravity( btVector3( 0.0, 0.0, 0.0 ) );
    return true;
}

bool World::SetGravity( const DrawSpace::Utils::Vector p_gravity )
{
    if( m_world )
    {
        m_world->setGravity( btVector3( p_gravity[0] * m_scale, p_gravity[1] * m_scale, p_gravity[2] * m_scale ) );
        return true;
    }
    return false;
}

bool World::StepSimulation( dsreal p_fps, int p_nbsteps )
{
    if( m_world )
    {

        /*
        // doing timestep < fixedtimestep * 5 (see http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World )
        
        dsreal timestep = 1.0 / p_fps;
        dsreal fixedtimestep = timestep / 5.0;
        fixedtimestep *= 1.1;

        m_world->stepSimulation( timestep, 5 );
        */

        m_world->stepSimulation( 1.0 / p_fps, p_nbsteps );

        // check for collisions


        for( std::map<btRigidBody*, Body*>::iterator it = m_bodies.begin(); it != m_bodies.end(); ++it )
        {
            (*it).second->SetContactState( false );
        }

        int numManifolds = m_world->getDispatcher()->getNumManifolds();

	    for (int i = 0; i < numManifolds; i++ )
	    {
		    btPersistentManifold* contactManifold =  m_world->getDispatcher()->getManifoldByIndexInternal(i);

            //btRigidBody* obA = static_cast<btRigidBody*>( contactManifold->getBody0() );

            btRigidBody* obA = (btRigidBody*)( contactManifold->getBody0() );
            btRigidBody* obB = (btRigidBody*)( contactManifold->getBody1() );

            int numContacts = contactManifold->getNumContacts();

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
	    }
        

        return true;
    }
    return false;
}

btDynamicsWorld* World::getBulletWorld( void )
{
    return m_world;
}

void World::AddBody( Body* p_body )
{
    //m_bodies.push_back( p_body );
    m_bodies[p_body->GetRigidBody()] = p_body;

    m_world->addRigidBody( p_body->GetRigidBody() );
}


void World::RemoveBody( Body* p_body )
{
    m_world->removeRigidBody( p_body->GetRigidBody() );
    for( std::map<btRigidBody*, Body*>::iterator it = m_bodies.begin(); it != m_bodies.end(); ++it )
    {
        if( (*it).second == p_body )
        {
            m_bodies.erase( it );
            break;
        }
    }
}

