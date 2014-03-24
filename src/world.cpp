/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;

World::World( void ) : m_world( NULL )
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
        m_world->setGravity( btVector3( p_gravity[0], p_gravity[1], p_gravity[2] ) );
        return true;
    }
    return false;
}

bool World::StepSimulation( dsreal p_fps )
{
    if( m_world )
    {

        // doing timestep < fixedtimestep * 5 (see http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World )
        
        dsreal timestep = 1.0 / p_fps;
        dsreal fixedtimestep = timestep / 5.0;
        fixedtimestep *= 1.1;

        m_world->stepSimulation( timestep, 5, fixedtimestep );

        return true;
    }
    return false;
}

btDynamicsWorld* World::getBulletWorld( void )
{
    return m_world;
}