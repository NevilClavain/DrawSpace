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

#include "physicsaspect.h"
#include "vector.h"
#include "timemanager.h"

#include "collisionaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;

PhysicsAspect::PhysicsAspect( void ) :
m_gravity_applied( false ),
m_time_aspect( NULL )
{
    m_collisionConfiguration            = _DRAWSPACE_NEW_( btDefaultCollisionConfiguration, btDefaultCollisionConfiguration );
    m_collisionDispatcher               = _DRAWSPACE_NEW_( btCollisionDispatcher, btCollisionDispatcher( m_collisionConfiguration ) );
    m_broadphase                        = _DRAWSPACE_NEW_( btDbvtBroadphase, btDbvtBroadphase );
    m_sequentialImpulseConstraintSolver = _DRAWSPACE_NEW_( btSequentialImpulseConstraintSolver, btSequentialImpulseConstraintSolver );
    m_world = _DRAWSPACE_NEW_( btDiscreteDynamicsWorld, btDiscreteDynamicsWorld( m_collisionDispatcher, m_broadphase, m_sequentialImpulseConstraintSolver, m_collisionConfiguration ) );

    m_world->setGravity( btVector3( 0.0, 0.0, 0.0 ) );
}

PhysicsAspect::~PhysicsAspect( void )
{
    _DRAWSPACE_DELETE_( m_world );    
    _DRAWSPACE_DELETE_( m_collisionConfiguration );
    _DRAWSPACE_DELETE_( m_collisionDispatcher );
    _DRAWSPACE_DELETE_( m_broadphase );
    _DRAWSPACE_DELETE_( m_sequentialImpulseConstraintSolver );
}

void PhysicsAspect::RegisterRigidBody(DrawSpace::Core::Entity* p_entity)
{
    BodyAspect* body_aspect = p_entity->GetAspect<BodyAspect>();

    if (!body_aspect)
    {
        _DSEXCEPTION("Entity has no body aspect")
    }

    btRigidBody* bd = body_aspect->Init();

    m_bodies[bd] = p_entity;
    body_aspect->RegisterPhysicalAspect(this);
}


void PhysicsAspect::UnregisterRigidBody( btRigidBody* p_rigidbody )
{
    if( m_bodies.count( p_rigidbody ) )
    {
        m_bodies.erase( p_rigidbody );
    }
}


void PhysicsAspect::RegisterCollider(DrawSpace::Core::Entity* p_entity)
{
    CollisionAspect* collision_aspect{ p_entity->GetAspect<CollisionAspect>() };
    if (!collision_aspect)
    {
        _DSEXCEPTION("Entity has no collision aspect")
    }

    btRigidBody* bd = collision_aspect->Init();

    m_bodies[bd] = p_entity;
    collision_aspect->RegisterPhysicalAspect(this);
}

void PhysicsAspect::UnregisterCollider(btRigidBody* p_rigidbody)
{
    if (m_bodies.count(p_rigidbody))
    {
        m_bodies.erase(p_rigidbody);
    }
}


void PhysicsAspect::SetTimeAspect( TimeAspect* p_time_aspect )
{
    m_time_aspect = p_time_aspect;
}

void PhysicsAspect::StepSimulation( void )
{
    if( NULL == m_time_aspect )
    {
        _DSEXCEPTION( "Physic world need a time reference!!!" )
    }

    ComponentList<int> ints;
    m_time_aspect->GetComponentsByType<int>( ints );

    ComponentList<TimeAspect::TimeScale> time_scales;
    m_time_aspect->GetComponentsByType<TimeAspect::TimeScale>( time_scales );

    ComponentList<TimeManager> tms;
    m_time_aspect->GetComponentsByType<TimeManager>( tms );

    TimeManager* tm = NULL;

    if( tms.size() > 0 )
    {
        tm = &tms[0]->getPurpose();
    }
    else
    {
         _DSEXCEPTION( "No Time manager associated with TimeAspect!!!" )
    }

    if( !tm->IsReady() )
    {
        return;
    }


    ComponentList<bool> flags;
    GetComponentsByType<bool>( flags );

    if( flags.size() )
    {
        bool enable_gravity = flags[0]->getPurpose();

        if( m_gravity_applied != enable_gravity )
        {
            m_gravity_applied = enable_gravity;

            if( m_gravity_applied )
            {
                Vector gravity;
                ComponentList<Vector> vecs;
                GetComponentsByType<Vector>( vecs );
                gravity = vecs[0]->getPurpose();
                
                m_world->setGravity( btVector3( gravity[0], gravity[1], gravity[2] ) );
            }
            else
            {
                m_world->setGravity( btVector3( 0.0, 0.0, 0.0 ) );
            }
        }    
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if( time_scales[0]->getPurpose() != TimeAspect::FREEZE )
    {
        ComponentList<dsreal> reals;
        m_time_aspect->GetComponentsByType<dsreal>( reals );



        // doing timestep < fixedtimestep * 5 (see http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World )
       
        int world_nb_steps = ints[1]->getPurpose();

        dsreal fps = (dsreal)tm->GetFPS() / reals[0]->getPurpose();;


        btScalar ts = 1.0 / fps;
        m_world->stepSimulation( ts, world_nb_steps );

        // check for collisions

        for( auto it = m_bodies.begin(); it != m_bodies.end(); ++it )
        {
            ComponentList<bool> bools;

            BodyAspect* body_aspect = it->second->GetAspect<BodyAspect>();
            if (body_aspect)
            {
                body_aspect->GetComponentsByType<bool>(bools);
            }
            else
            {
                CollisionAspect* coll_aspect = it->second->GetAspect<CollisionAspect>();
                coll_aspect->GetComponentsByType<bool>(bools);
            }
           
            if( bools.size() < 1 )
            {
                _DSEXCEPTION( "Bad number of bool components for BodyAspect" )
            }

            // reset du composant "contactState" (bools[0])

            if( bools.size() > 0 )
            {
                bools[0]->getPurpose() = false;
            }
        }

        int numManifolds = m_world->getDispatcher()->getNumManifolds();

	    for (int i = 0; i < numManifolds; i++ )
	    {
		    btPersistentManifold* contactManifold =  m_world->getDispatcher()->getManifoldByIndexInternal(i);

            btRigidBody* obA = (btRigidBody*)( contactManifold->getBody0() );
            btRigidBody* obB = (btRigidBody*)( contactManifold->getBody1() );

            int numContacts = contactManifold->getNumContacts();
            
            if( m_bodies.count( obA ) > 0 )
            {
                if( numContacts > 0 )
                {
                    ComponentList<bool> bools;

                    BodyAspect* body_aspect = m_bodies[obA]->GetAspect<BodyAspect>();
                    if (body_aspect)
                    {
                        body_aspect->GetComponentsByType<bool>(bools);
                    }
                    else
                    {
                        CollisionAspect* coll_aspect = m_bodies[obA]->GetAspect<CollisionAspect>();
                        coll_aspect->GetComponentsByType<bool>(bools);
                    }
                    
                    if (bools.size() > 0)
                    {
                        bools[0]->getPurpose() = true;
                    }
                }
            }
            if( m_bodies.count( obB ) > 0 )
            {
                if( numContacts > 0 )
                {
                    ComponentList<bool> bools;

                    BodyAspect* body_aspect = m_bodies[obB]->GetAspect<BodyAspect>();

                    if (body_aspect)
                    {
                        body_aspect->GetComponentsByType<bool>(bools);
                    }
                    else
                    {
                        CollisionAspect* coll_aspect = m_bodies[obB]->GetAspect<CollisionAspect>();
                        coll_aspect->GetComponentsByType<bool>(bools);
                    }
                    
                    if (bools.size() > 0)
                    {
                        bools[0]->getPurpose() = true;
                    }
                }
            }            
	    }
    }
}

btDiscreteDynamicsWorld* PhysicsAspect::GetWorld( void ) const
{
    return m_world;
}


