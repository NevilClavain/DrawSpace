/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include "transformaspect.h"
#include "collisionaspect.h"
#include "rigidbodytransformaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Interface::AspectImplementations;

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

RigidBodyTransformAspectImpl* PhysicsAspect::find_rigidbodytransformaspectimpl(DrawSpace::Core::Entity* p_entity) const
{
    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
    if (transform_aspect)
    {
        RigidBodyTransformAspectImpl* rigibody_transform_impl{ nullptr };
        auto transformimpls_list{ transform_aspect->GetTransformAspectImplsList() };

        for (auto& e : transformimpls_list)
        {
            TransformAspectImpl* impl = e.second;
            rigibody_transform_impl = dynamic_cast<RigidBodyTransformAspectImpl*>(impl);

            if (rigibody_transform_impl)
            {
                break;
            }
        }
        if (!rigibody_transform_impl)
        {
            _DSEXCEPTION("Entity has no rigidbody transform")
        }
        else
        {
            return rigibody_transform_impl;
        }
    }
    else
    {
        _DSEXCEPTION("Entity has no transform aspect")
    }
}

void PhysicsAspect::RegisterRigidBody(DrawSpace::Core::Entity* p_entity)
{
    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
    if (transform_aspect)
    {
        RigidBodyTransformAspectImpl* rigibody_transform_impl{ find_rigidbodytransformaspectimpl(p_entity) };
        btRigidBody* bd{ rigibody_transform_impl->Init(transform_aspect) };

        m_bodies[bd] = p_entity;
        rigibody_transform_impl->RegisterPhysicalAspect(this);
    }
    else
    {
        _DSEXCEPTION("Entity has no transform aspect")
    }
}


void PhysicsAspect::UnregisterRigidBody(DrawSpace::Core::Entity* p_entity)
{
    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
    if (transform_aspect)
    {
        RigidBodyTransformAspectImpl* rigibody_transform_impl{ find_rigidbodytransformaspectimpl(p_entity) };
        btRigidBody* bd{ rigibody_transform_impl->GetRigidBody() };
        if (m_bodies.count(bd))
        {
            m_bodies.erase(bd);
        }
        rigibody_transform_impl->Release();
    }
    else
    {
        _DSEXCEPTION("Entity has no transform aspect")
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

void PhysicsAspect::UnregisterCollider(DrawSpace::Core::Entity* p_entity)
{
    CollisionAspect* collision_aspect{ p_entity->GetAspect<CollisionAspect>() };
    if (!collision_aspect)
    {
        _DSEXCEPTION("Entity has no collision aspect")
    }
   
    btRigidBody* bd{ collision_aspect->GetRigidBody() };
    if (m_bodies.count(bd))
    {
        m_bodies.erase(bd);
    }

    collision_aspect->Release();
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

    if( time_scales[0]->getPurpose() != TimeAspect::TimeScale::FREEZE )
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

            CollisionAspect* coll_aspect = it->second->GetAspect<CollisionAspect>();
            if (coll_aspect)
            {
                coll_aspect->GetComponentsByType<bool>(bools);
            }
            else
            {
                TransformAspect* transf_aspect = it->second->GetAspect<TransformAspect>();
                if (transf_aspect)
                {
                    transf_aspect->GetComponentsByType<bool>(bools);
                }
                else
                {
                    _DSEXCEPTION("No suitable aspect found for this entity")
                }
            }
           
            if( bools.size() < 1 )
            {
                _DSEXCEPTION( "Bad number of bool components for CollisionAspect" )
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

                    CollisionAspect* coll_aspect{ m_bodies[obA]->GetAspect<CollisionAspect>() };
                    if (coll_aspect)
                    {
                        coll_aspect->GetComponentsByType<bool>(bools);
                    }
                    else
                    {
                        TransformAspect* transf_aspect = m_bodies[obA]->GetAspect<TransformAspect>();
                        if (transf_aspect)
                        {
                            transf_aspect->GetComponentsByType<bool>(bools);
                        }
                        else
                        {
                            _DSEXCEPTION("No suitable aspect found for this entity")
                        }
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

                    CollisionAspect* coll_aspect{ m_bodies[obB]->GetAspect<CollisionAspect>() };

                    if (coll_aspect)
                    {
                        coll_aspect->GetComponentsByType<bool>(bools);
                    }
                    else
                    {
                        TransformAspect* transf_aspect = m_bodies[obB]->GetAspect<TransformAspect>();
                        if (transf_aspect)
                        {
                            transf_aspect->GetComponentsByType<bool>(bools);
                        }
                        else
                        {
                            _DSEXCEPTION("No suitable aspect found for this entity")
                        }
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


