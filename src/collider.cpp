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

Collider::Collider( World* p_world ) : Body( p_world )
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

void Collider::Update( const Matrix& p_mat )
{
    m_lastworldtrans = p_mat;
}


btRigidBody* Collider::GetRigidBody( void )
{
    return m_rigidBody;
}

void Collider::AddToWorld( void )
{
    m_world->AddBody( this );
}

void Collider::RemoveFromWorld( void )
{
    m_world->RemoveBody( this );
}
