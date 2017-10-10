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

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;

PhysicsAspect::PhysicsAspect( void ) :
m_collisionDispatcher( &m_collisionConfiguration ),
m_world( &m_collisionDispatcher, &m_broadphase, &m_sequentialImpulseConstraintSolver, &m_collisionConfiguration)
{
}

void PhysicsAspect::StepSimulation( dsreal p_fps, int p_nbsteps )
{
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