/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;

PhysicsAspect::PhysicsAspect( void ) :
m_gravity_applied( false ),
//m_tm( NULL )
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

void PhysicsAspect::on_added_bodyentity( Entity* p_entity )
{
    BodyAspect* body_aspect = p_entity->GetAspect<BodyAspect>();

    // non ce n'est pas au systeme d'initialiser le body aspect
    //btRigidBody* bd = body_aspect->Init();

    btRigidBody* bd = body_aspect->GetRigidBody();

    if( NULL == bd )
    {
        _DSEXCEPTION( "body aspect not initialized" )
    }

    m_bodies[bd] = p_entity;

    //m_world->addRigidBody( bd );
    body_aspect->RegisterPhysicalAspect( this );
 }

void PhysicsAspect::on_removed_bodyentity( Entity* p_entity )
{
    BodyAspect* body_aspect = p_entity->GetAspect<BodyAspect>();

    if( body_aspect )
    {
        btRigidBody* bd = body_aspect->GetRigidBody();

        if( bd )
        {
            // maintenant fait dans body_aspect->Release() ou body_aspect->RegisterToWorld
            // si le body passe d'un monde a l'autre (attachment/detachment)

            //m_world->removeRigidBody( bd );

            m_bodies.erase( bd );

            // non ce n'est pas au systeme de releaser le body aspect
            //body_aspect->Release();
        }
    }
}

void PhysicsAspect::UpdateBodiesList( const std::set<Entity*>& p_list )
{    
    for( auto it = p_list.begin(); it != p_list.end(); ++it )
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
    for( auto it = m_bodies_set.begin(); it != m_bodies_set.end(); ++it )
    {
        //chercher dans p_list...
        Entity* curr_entity = *it;

        if( 0 == p_list.count( curr_entity ) )
        {
            // cette entite n'est pas dans la liste fournie en entree -> a ete retiree du graph
            on_removed_bodyentity( curr_entity );
            to_remove.push_back( curr_entity );
        }
    }

    for( size_t i = 0; i < to_remove.size(); ++i )
    {
        // effacer de m_bodies_set
        m_bodies_set.erase( to_remove[i] );

        /* FAIT DANS on_removed_bodyentity
        // effacer de m_bodies
        for( auto it = m_bodies.begin(); it != m_bodies.end(); ++it )
        {
            if( (*it).second == to_remove[i] )
            {
                m_bodies.erase( it );
                break;
            }
        }
        */
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
            BodyAspect* body_aspect = it->second->GetAspect<BodyAspect>();

            ComponentList<bool> bools;
            body_aspect->GetComponentsByType<bool>( bools );

            if( bools.size() < 2 )
            {
                _DSEXCEPTION( "Bad number of bool components for BodyAspect" )
            }

            // reset du composant "contactState" (bools[1])
            bools[1]->getPurpose() = false;
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
                    BodyAspect* body_aspect = m_bodies[obA]->GetAspect<BodyAspect>();

                    ComponentList<bool> bools;
                    body_aspect->GetComponentsByType<bool>( bools );

                    bools[1]->getPurpose() = true;
                }
            }
            if( m_bodies.count( obB ) > 0 )
            {
                if( numContacts > 0 )
                {
                    BodyAspect* body_aspect = m_bodies[obB]->GetAspect<BodyAspect>();

                    ComponentList<bool> bools;
                    body_aspect->GetComponentsByType<bool>( bools );

                    bools[1]->getPurpose() = true;
                }
            }            
	    }
    }
}

btDiscreteDynamicsWorld* PhysicsAspect::GetWorld( void ) const
{
    return m_world;
}

void PhysicsAspect::UnregisterRigidBody( btRigidBody* p_rigidbody )
{
    if( m_bodies.count( p_rigidbody ) )
    {
        m_bodies_set.erase( m_bodies[p_rigidbody] );
        m_bodies.erase( p_rigidbody );
    }
}