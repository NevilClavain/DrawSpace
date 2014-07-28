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

#include "collider.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;

Collider::Collider( DrawSpace::Core::TransformNode* p_drawable ) : Body( /*p_world*/ NULL ),
m_drawable( p_drawable ),
m_orbiter( NULL ),
m_movement( NULL )
{

}

Collider::~Collider( void )
{

}

void Collider::SetKinematic( const Body::Parameters& p_parameters )
{
    dsreal world_scale = World::m_scale;

    btTransform bt_transform;

    btScalar btmat[16];

    btmat[0] = p_parameters.initial_attitude( 0, 0 );
    btmat[1] = p_parameters.initial_attitude( 0, 1 );
    btmat[2] = p_parameters.initial_attitude( 0, 2 );
    btmat[3] = p_parameters.initial_attitude( 0, 3 );

    btmat[4] = p_parameters.initial_attitude( 1, 0 );
    btmat[5] = p_parameters.initial_attitude( 1, 1 );
    btmat[6] = p_parameters.initial_attitude( 1, 2 );
    btmat[7] = p_parameters.initial_attitude( 1, 3 );

    btmat[8] = p_parameters.initial_attitude( 2, 0 );
    btmat[9] = p_parameters.initial_attitude( 2, 1 );
    btmat[10] = p_parameters.initial_attitude( 2, 2 );
    btmat[11] = p_parameters.initial_attitude( 2, 3 );

    btmat[12] = p_parameters.initial_attitude( 3, 0 );
    btmat[13] = p_parameters.initial_attitude( 3, 1 );
    btmat[14] = p_parameters.initial_attitude( 3, 2 );
    btmat[15] = p_parameters.initial_attitude( 3, 3 );

    bt_transform.setFromOpenGLMatrix( btmat ); 


    m_collisionShape = instanciate_collision_shape( p_parameters.shape_descr, &m_meshe_data );

    m_motionState = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( bt_transform ) );

    btVector3 localInertia( 0, 0, 0 );

    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo( 0.0, m_motionState, m_collisionShape, localInertia );
    m_rigidBody = _DRAWSPACE_NEW_( btRigidBody, btRigidBody( boxRigidBodyConstructionInfo ) );

    // switch the body to kinematic mode
    m_rigidBody->setCollisionFlags( m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
    m_rigidBody->setActivationState( DISABLE_DEACTIVATION );
}

void Collider::UnsetKinematic( void )
{

    _DRAWSPACE_DELETE_( m_rigidBody );
    _DRAWSPACE_DELETE_( m_collisionShape );
    _DRAWSPACE_DELETE_( m_motionState );

    if( m_meshe_data )
    {
        _DRAWSPACE_DELETE_( m_meshe_data );
    }
}

void Collider::Update( Utils::TimeManager& p_timemanager, const Matrix& p_mat )
{
    Matrix body_transf;

    if( m_movement )
    {
        m_movement->Compute( p_timemanager );
        m_movement->GetResult( body_transf );
    }
    else
    {
        body_transf = p_mat;
    }
   
    // update meshe bullet

    btScalar btmat[16];
    btTransform bt_transform;

    btmat[0] = body_transf( 0, 0 );
    btmat[1] = body_transf( 0, 1 );
    btmat[2] = body_transf( 0, 2 );
    btmat[3] = body_transf( 0, 3 );

    btmat[4] = body_transf( 1, 0 );
    btmat[5] = body_transf( 1, 1 );
    btmat[6] = body_transf( 1, 2 );
    btmat[7] = body_transf( 1, 3 );

    btmat[8] = body_transf( 2, 0 );
    btmat[9] = body_transf( 2, 1 );
    btmat[10] = body_transf( 2, 2 );
    btmat[11] = body_transf( 2, 3 );

    btmat[12] = body_transf( 3, 0 );
    btmat[13] = body_transf( 3, 1 );
    btmat[14] = body_transf( 3, 2 );
    btmat[15] = body_transf( 3, 3 );

    bt_transform.setFromOpenGLMatrix( btmat ); 

    m_motionState->setWorldTransform( bt_transform );

    if( m_orbiter )
    {
        Matrix root_mat;
        m_orbiter->GetLastWorldTransformation( root_mat );

        m_lastworldtrans = body_transf * root_mat;
    }
    else
    {
        m_lastworldtrans = body_transf;
    }

    if( m_drawable )
    {
         m_drawable->SetLocalTransform( m_lastworldtrans );
    }

    // update position drawable
    /*
    if( m_drawable )
    {
        if( m_orbiter )
        {
            Matrix root_mat;
            m_orbiter->GetLastWorldTransformation( root_mat );

            Matrix res = m_lastworldtrans * root_mat;
            m_drawable->SetLocalTransform( res );            
        }
        else
        {
            m_drawable->SetLocalTransform( m_lastworldtrans );
        }
    }
    */
}

btRigidBody* Collider::GetRigidBody( void )
{
    return m_rigidBody;
}

void Collider::AddToWorld( World* p_world )
{
    m_world = p_world;
    m_world->AddBody( this );
}

void Collider::RemoveFromWorld( void )
{
    m_world->RemoveBody( this );
}

void Collider::SetRootOrbiter( Orbiter* p_orbiter )
{
    m_orbiter = p_orbiter;
}

void Collider::RegisterMovement( DrawSpace::Core::Movement* p_movement )
{
    m_movement = p_movement;
}