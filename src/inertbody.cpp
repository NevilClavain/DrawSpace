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


InertBody::InertBody( World* p_world, DrawSpace::Interface::Drawable* p_drawable, const Body::Parameters& p_parameters ) : Body( p_world, p_drawable ),
m_refbody( NULL )
{
    m_global_world_mem = m_world;
    btTransform bt_transform;

    bt_transform.setIdentity();
    bt_transform.setOrigin( btVector3( m_parameters.initial_pos[0], m_parameters.initial_pos[1], m_parameters.initial_pos[2] ) );

    create_body( bt_transform );
}

InertBody::~InertBody( void )
{
    destroy_body();
}


void InertBody::create_body( const btTransform& p_transform )
{
    m_collisionShape = instanciate_collision_shape( m_parameters.shape_descr );

    m_motionState = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( p_transform ) );

    btVector3 localInertia( 0, 0, 0 );
    if( m_parameters.mass > 0.0 )
    {        
        m_collisionShape->calculateLocalInertia( m_parameters.mass, localInertia );
    }

    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo( m_parameters.mass, m_motionState, m_collisionShape, localInertia );
    m_rigidBody = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( boxRigidBodyConstructionInfo ) );

    m_world->getBulletWorld()->addRigidBody( m_rigidBody );

    m_rigidBody->setActivationState( DISABLE_DEACTIVATION );
}

void InertBody::destroy_body( void )
{
    m_world->getBulletWorld()->removeRigidBody( m_rigidBody );
    _DRAWSPACE_DELETE_( m_motionState );
    _DRAWSPACE_DELETE_( m_collisionShape );
    _DRAWSPACE_DELETE_( m_rigidBody );
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

    m_lastworldtrans = updated_matrix;
}


/*

mat_a => matrice du body a attacher (exemple : ship)
mat_b => matrice du body auxquel on s'attache (exemple : ship)

   1/ attachement :
    
        a: mat_a2 = mat_a * ( mat_b ^ -1 )
        b: detruire puis recreer le body en lui donnant mat_a2 comme matrice initiale


    2/ pendant la phase d'attachement :

        body attaché : injecter dans le drawable (via SetLocalTransform()) la matrice transfo suivante :
                    
                        mat_body * mat_b

                        avec mat_body = la transfo du body calculée par bullet

                        (au tout debut de l'attachement, mat_body = mat_a2)



    3/ detachement

        a: mat_a3 = mat_body * mat_b
        b: detruire puis recreer le body en lui donnant mat_a3 comme matrice initiale


*/

void InertBody::Attach( Body* p_body )
{
    if( m_refbody )
    {
        return;
    }

    // recup derniere transfo body auquel on s'attache
    Matrix mat_b = m_lastworldtrans;
    mat_b.Inverse();

    DrawSpace::Utils::Matrix mat_a2 = m_lastworldtrans * mat_b;
    
    // memoriser mat_a2, pour le reinjecter en transfo initiale pour le nouveau body
    btScalar kmat[16];    
    btTransform tf_a2;

    kmat[0] = mat_a2( 0, 0 );
    kmat[1] = mat_a2( 0, 1 );
    kmat[2] = mat_a2( 0, 2 );
    kmat[3] = mat_a2( 0, 3 );

    kmat[4] = mat_a2( 1, 0 );
    kmat[5] = mat_a2( 1, 1 );
    kmat[6] = mat_a2( 1, 2 );
    kmat[7] = mat_a2( 1, 3 );

    kmat[8] = mat_a2( 2, 0 );
    kmat[9] = mat_a2( 2, 1 );
    kmat[10] = mat_a2( 2, 2 );
    kmat[11] = mat_a2( 2, 3 );

    kmat[12] = mat_a2( 3, 0 );
    kmat[13] = mat_a2( 3, 1 );
    kmat[14] = mat_a2( 3, 2 );
    kmat[15] = mat_a2( 3, 3 );

    tf_a2.setFromOpenGLMatrix( kmat );

    ///////////////////////////////////////////////////////

    btVector3  linearspeed_mem;
    btVector3  angularspeed_mem;

    linearspeed_mem = m_rigidBody->getLinearVelocity();
    angularspeed_mem = m_rigidBody->getAngularVelocity();


    // detruire le body...
    destroy_body();

    // on change de 'monde' bullet
    m_world = p_body->GetWorld();

    // recreer le body...
    create_body( tf_a2 );

    m_refbody = p_body;
}

void InertBody::Detach( void )
{
    if( !m_refbody )
    {
        return;
    }
    // on revient dans le monde bullet global...
    m_world = m_global_world_mem;

    m_refbody = NULL;
}
