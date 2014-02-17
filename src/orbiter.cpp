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

#include "orbiter.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


void Orbiter::Orbit::Compute( dsreal p_angle, DrawSpace::Utils::Vector& p_respoint )
{
    dsreal a = 1.0;
    dsreal b = m_excentricity * a;

    dsreal rad_ang = Maths::DegToRad( p_angle );

    dsreal x = a * cos( rad_ang );
    dsreal y = b * sin( rad_ang );

    x = ( x * m_ray ) + m_offset_plane_x;
    y = ( y * m_ray ) + m_offset_plane_y;

    p_respoint[0] = x;
    p_respoint[1] = 0.0;
    p_respoint[2] = y;
}

Orbiter::Orbiter( World* p_world, DrawSpace::Interface::Drawable* p_drawable ) : Body( p_world, p_drawable ),
m_rigidBody( NULL ),
m_collisionShape( NULL ),
m_motionState( NULL ),
m_parent( NULL )
{

}

Orbiter::~Orbiter( void )
{
}

bool Orbiter::SetKinematic( const Parameters& p_parameters )
{
    btTransform bt_transform;

    bt_transform.setIdentity();
    bt_transform.setOrigin( btVector3( p_parameters.initial_pos[0], p_parameters.initial_pos[1], p_parameters.initial_pos[2] ) );

    m_collisionShape = instanciate_collision_shape( p_parameters.shape_descr );

    m_motionState = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( bt_transform ) );

    btVector3 localInertia( 0, 0, 0 );

    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo( 0.0, m_motionState, m_collisionShape, localInertia );
    m_rigidBody = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( boxRigidBodyConstructionInfo ) );

    // switch the body to kinematic mode
    m_rigidBody->setCollisionFlags( m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
    m_rigidBody->setActivationState( DISABLE_DEACTIVATION );

    m_world->getBulletWorld()->addRigidBody( m_rigidBody );

    return true;
}

bool Orbiter::UnsetKinematic( void )
{
    m_world->getBulletWorld()->removeRigidBody( m_rigidBody );

    _DRAWSPACE_DELETE_( m_rigidBody );
    _DRAWSPACE_DELETE_( m_collisionShape );
    _DRAWSPACE_DELETE_( m_motionState );

    return true;
}

void Orbiter::Update( dsreal p_angle, const Vector& p_centroid )
{

    Vector orbit1_point;
    m_orbit_1.Compute( p_angle, orbit1_point );

    
    
    Matrix orbiter_trans;
    orbiter_trans.Translation( orbit1_point[0] + p_centroid[0], 
                                orbit1_point[1] + p_centroid[1],
                                orbit1_point[2] + p_centroid[2] );

    btScalar kmat[16];    
    btTransform ktf;

    kmat[0] = orbiter_trans( 0, 0 );
    kmat[1] = orbiter_trans( 0, 1 );
    kmat[2] = orbiter_trans( 0, 2 );
    kmat[3] = orbiter_trans( 0, 3 );

    kmat[4] = orbiter_trans( 1, 0 );
    kmat[5] = orbiter_trans( 1, 1 );
    kmat[6] = orbiter_trans( 1, 2 );
    kmat[7] = orbiter_trans( 1, 3 );

    kmat[8] = orbiter_trans( 2, 0 );
    kmat[9] = orbiter_trans( 2, 1 );
    kmat[10] = orbiter_trans( 2, 2 );
    kmat[11] = orbiter_trans( 2, 3 );

    kmat[12] = orbiter_trans( 3, 0 );
    kmat[13] = orbiter_trans( 3, 1 );
    kmat[14] = orbiter_trans( 3, 2 );
    kmat[15] = orbiter_trans( 3, 3 );

    ktf.setFromOpenGLMatrix( kmat );
    m_motionState->setWorldTransform( ktf );

    m_drawable->SetLocalTransform( orbiter_trans );

    
    for( size_t i = 0; i < m_children.size(); i++ )
    {
        m_children[i]->Update( p_angle, orbit1_point );
    }
}

void Orbiter::AddChild( Orbiter* p_orbiter )
{
    m_children.push_back( p_orbiter );
    p_orbiter->m_parent = this;
}

void Orbiter::SetOrbit1( const Orbit& p_orbit )
{
    m_orbit_1 = p_orbit;
}

void Orbiter::SetOrbit2( const Orbit& p_orbit )
{
    m_orbit_2 = p_orbit;
}
