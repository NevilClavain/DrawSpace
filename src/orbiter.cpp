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

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;

Orbiter::Orbiter( World* p_world, DrawSpace::Interface::Drawable* p_drawable ) : Body( p_world, p_drawable ),
m_rigidBody( NULL ),
m_collisionShape( NULL ),
m_motionState( NULL )
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

    switch( p_parameters.shape )
    {
        case Body::BOX_SHAPE:
            m_collisionShape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( p_parameters.box_dims[0], p_parameters.box_dims[1], p_parameters.box_dims[2] ) ) );
            break;
    }

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
