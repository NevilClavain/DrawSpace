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

#include "inertbody.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


InertBody::InertBody( World* p_world, DrawSpace::Interface::Drawable* p_drawable, const Parameters& p_parameters ) : Body( p_world, p_drawable )
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
    if( p_parameters.mass > 0.0 )
    {        
        m_collisionShape->calculateLocalInertia( p_parameters.mass, localInertia );
    }

    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo( p_parameters.mass, m_motionState, m_collisionShape, localInertia );
    m_rigidBody = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( boxRigidBodyConstructionInfo ) );

    m_world->getBulletWorld()->addRigidBody( m_rigidBody );
}

InertBody::~InertBody( void )
{
    _DRAWSPACE_DELETE_( m_rigidBody );
    _DRAWSPACE_DELETE_( m_collisionShape );
    _DRAWSPACE_DELETE_( m_motionState );
}

void InertBody::GetParameters( Parameters& p_parameters )
{
    p_parameters = m_parameters;
}

void InertBody::Update( void )
{
    btScalar                 bt_matrix[16];
    DrawSpace::Utils::Matrix updated_matrix;

    m_motionState->m_graphicsWorldTrans.getOpenGLMatrix( bt_matrix );
   
    updated_matrix( 0, 0 ) = bt_matrix[0];
    updated_matrix( 0, 1 ) = bt_matrix[1];
    updated_matrix( 0, 2 ) = bt_matrix[2];
    updated_matrix( 0, 3 ) = bt_matrix[3];

    updated_matrix( 1, 0 ) = bt_matrix[4];
    updated_matrix( 1, 1 ) = bt_matrix[5];
    updated_matrix( 1, 2 ) = bt_matrix[6];
    updated_matrix( 1, 3 ) = bt_matrix[7];

    updated_matrix( 2, 0 ) = bt_matrix[8];
    updated_matrix( 2, 1 ) = bt_matrix[9];
    updated_matrix( 2, 2 ) = bt_matrix[10];
    updated_matrix( 2, 3 ) = bt_matrix[11];

    updated_matrix( 3, 0 ) = bt_matrix[12];
    updated_matrix( 3, 1 ) = bt_matrix[13];
    updated_matrix( 3, 2 ) = bt_matrix[14];
    updated_matrix( 3, 3 ) = bt_matrix[15];

    m_drawable->SetLocalTransform( updated_matrix );
}