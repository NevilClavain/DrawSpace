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


InertBody::InertBody( World* p_world, TransformNode* p_drawable, const Body::Parameters& p_parameters ) : Body( p_world, p_drawable ),
m_refbody( NULL ),
m_parameters( p_parameters ),
m_rigidBody( NULL ),
m_collisionShape( NULL ),
m_motionState( NULL ),
m_meshe_data( NULL )
{
    m_global_world_mem = m_world;

    dsreal world_scale = World::m_scale;
    btTransform bt_transform;

    m_lastlocalworldtrans.Identity();

    /*
    bt_transform.setIdentity();
    bt_transform.setOrigin( btVector3( m_parameters.initial_pos[0] * world_scale, m_parameters.initial_pos[1] * world_scale, m_parameters.initial_pos[2] * world_scale ) );
    */

    btScalar btmat[16];

    btmat[0] = m_parameters.initial_attitude( 0, 0 );
    btmat[1] = m_parameters.initial_attitude( 0, 1 );
    btmat[2] = m_parameters.initial_attitude( 0, 2 );
    btmat[3] = m_parameters.initial_attitude( 0, 3 );

    btmat[4] = m_parameters.initial_attitude( 1, 0 );
    btmat[5] = m_parameters.initial_attitude( 1, 1 );
    btmat[6] = m_parameters.initial_attitude( 1, 2 );
    btmat[7] = m_parameters.initial_attitude( 1, 3 );

    btmat[8] = m_parameters.initial_attitude( 2, 0 );
    btmat[9] = m_parameters.initial_attitude( 2, 1 );
    btmat[10] = m_parameters.initial_attitude( 2, 2 );
    btmat[11] = m_parameters.initial_attitude( 2, 3 );

    btmat[12] = m_parameters.initial_attitude( 3, 0 );
    btmat[13] = m_parameters.initial_attitude( 3, 1 );
    btmat[14] = m_parameters.initial_attitude( 3, 2 );
    btmat[15] = m_parameters.initial_attitude( 3, 3 );

    bt_transform.setFromOpenGLMatrix( btmat ); 
   
    create_body( bt_transform );
}

InertBody::~InertBody( void )
{
    destroy_body();
}


void InertBody::create_body( const btTransform& p_transform )
{
    dsreal world_scale = World::m_scale;

    m_collisionShape = instanciate_collision_shape( m_parameters.shape_descr, &m_meshe_data );

    m_motionState = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( p_transform ) );

    btVector3 localInertia( 0, 0, 0 );
    if( m_parameters.mass > 0.0 )
    {        
        m_collisionShape->calculateLocalInertia( m_parameters.mass * world_scale, localInertia );
    }

    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo( m_parameters.mass * world_scale, m_motionState, m_collisionShape, localInertia );
    m_rigidBody = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( boxRigidBodyConstructionInfo ) );

    //m_world->getBulletWorld()->addRigidBody( m_rigidBody );
    m_world->AddBody( this );

    m_rigidBody->setActivationState( DISABLE_DEACTIVATION );
}

void InertBody::destroy_body( void )
{
    //m_world->getBulletWorld()->removeRigidBody( m_rigidBody );
    m_world->RemoveBody( this );
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
    dsreal world_scale = World::m_scale;

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

    updated_matrix( 3, 0 ) = bt_matrix[12] / world_scale;
    updated_matrix( 3, 1 ) = bt_matrix[13] / world_scale;
    updated_matrix( 3, 2 ) = bt_matrix[14] / world_scale;
    updated_matrix( 3, 3 ) = bt_matrix[15];

    m_lastlocalworldtrans = updated_matrix;

    if( NULL == m_refbody )
    {
        // not attached
        m_drawable->SetLocalTransform( updated_matrix );
        m_lastworldtrans = updated_matrix;
    }
    else
    {
        // attached : ajouter la transfo du body auquel on est attache
        DrawSpace::Utils::Matrix mat_b;
        m_refbody->GetLastWorldTransformation( mat_b );

        DrawSpace::Utils::Matrix res = updated_matrix * mat_b;

        m_drawable->SetLocalTransform( res );
        m_lastworldtrans = res;
    }
}


/*

mat_a => matrice du body a attacher (exemple : ship)
mat_b => matrice du body auxquel on s'attache (exemple : planete)

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

    dsreal world_scale = World::m_scale;

    // recup derniere transfo body auquel on s'attache
    Matrix mat_b;
    p_body->GetLastWorldTransformation( mat_b );
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

    kmat[12] = mat_a2( 3, 0 ) * world_scale;
    kmat[13] = mat_a2( 3, 1 ) * world_scale;
    kmat[14] = mat_a2( 3, 2 ) * world_scale;
    kmat[15] = mat_a2( 3, 3 );

    tf_a2.setFromOpenGLMatrix( kmat );

    ///////////////////////////////////////////////////////

    btVector3 bt_linearspeed_mem;
    btVector3 bt_angularspeed_mem;

    bt_linearspeed_mem = m_rigidBody->getLinearVelocity();
    bt_angularspeed_mem = m_rigidBody->getAngularVelocity();

    ///////////////////////////////////////////////////////

    // detruire le body...
    destroy_body();

    // on passe dans le 'monde' bullet local de p_body
    m_world = p_body->GetWorld();

    // recreer le body...
    create_body( tf_a2 );


    // bt_angularspeed_mem & bt_linearspeed_mem a passer dans le repere body auquel on s'attache

    Vector angularspeed_mem, linearspeed_mem;
    Vector angularspeed_mem_2, linearspeed_mem_2;

    angularspeed_mem[0] = bt_angularspeed_mem.x();
    angularspeed_mem[1] = bt_angularspeed_mem.y();
    angularspeed_mem[2] = bt_angularspeed_mem.z();
    angularspeed_mem[3] = 1.0;


    linearspeed_mem[0] = bt_linearspeed_mem.x();
    linearspeed_mem[1] = bt_linearspeed_mem.y();
    linearspeed_mem[2] = bt_linearspeed_mem.z();
    linearspeed_mem[3] = 1.0;

    mat_b.ClearTranslation();

    mat_b.Transform( &angularspeed_mem, &angularspeed_mem_2 );
    mat_b.Transform( &linearspeed_mem, &linearspeed_mem_2 );
   
    m_rigidBody->setAngularVelocity( btVector3( angularspeed_mem_2[0], angularspeed_mem_2[1], angularspeed_mem_2[2] ) );
    m_rigidBody->setLinearVelocity( btVector3( linearspeed_mem_2[0], linearspeed_mem_2[1], linearspeed_mem_2[2] ) );

    ////////

    m_refbody = p_body;

    for( std::vector<EventHandler*>::iterator it = m_evt_handlers.begin(); it != m_evt_handlers.end(); ++it )
    {
        ( **it )( ATTACHED, m_refbody );
    }
}

/*

 Identique a Attach, sauf qu'ici mat_a2 = matrice 'initiale' fournit en argument

*/

void InertBody::IncludeTo( Body* p_body, const Matrix& p_initmat )
{
    if( m_refbody )
    {
        return;
    }

    dsreal world_scale = World::m_scale;

    // recup derniere transfo body auquel on s'attache
    Matrix mat_b;
    p_body->GetLastWorldTransformation( mat_b );
    mat_b.Inverse();

    DrawSpace::Utils::Matrix mat_a2 = p_initmat;
    
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

    kmat[12] = mat_a2( 3, 0 ) * world_scale;
    kmat[13] = mat_a2( 3, 1 ) * world_scale;
    kmat[14] = mat_a2( 3, 2 ) * world_scale;
    kmat[15] = mat_a2( 3, 3 );

    tf_a2.setFromOpenGLMatrix( kmat );

    ///////////////////////////////////////////////////////

    btVector3 bt_linearspeed_mem;
    btVector3 bt_angularspeed_mem;

    bt_linearspeed_mem = m_rigidBody->getLinearVelocity();
    bt_angularspeed_mem = m_rigidBody->getAngularVelocity();

    ///////////////////////////////////////////////////////

    // detruire le body...
    destroy_body();

    // on passe dans le 'monde' bullet local de p_body
    m_world = p_body->GetWorld();

    // recreer le body...
    create_body( tf_a2 );


    // bt_angularspeed_mem & bt_linearspeed_mem a passer dans le repere body auquel on s'attache

    Vector angularspeed_mem, linearspeed_mem;
    Vector angularspeed_mem_2, linearspeed_mem_2;

    angularspeed_mem[0] = bt_angularspeed_mem.x();
    angularspeed_mem[1] = bt_angularspeed_mem.y();
    angularspeed_mem[2] = bt_angularspeed_mem.z();
    angularspeed_mem[3] = 1.0;


    linearspeed_mem[0] = bt_linearspeed_mem.x();
    linearspeed_mem[1] = bt_linearspeed_mem.y();
    linearspeed_mem[2] = bt_linearspeed_mem.z();
    linearspeed_mem[3] = 1.0;

    mat_b.ClearTranslation();

    mat_b.Transform( &angularspeed_mem, &angularspeed_mem_2 );
    mat_b.Transform( &linearspeed_mem, &linearspeed_mem_2 );
   
    m_rigidBody->setAngularVelocity( btVector3( angularspeed_mem_2[0], angularspeed_mem_2[1], angularspeed_mem_2[2] ) );
    m_rigidBody->setLinearVelocity( btVector3( linearspeed_mem_2[0], linearspeed_mem_2[1], linearspeed_mem_2[2] ) );

    ////////

    m_refbody = p_body;

    for( std::vector<EventHandler*>::iterator it = m_evt_handlers.begin(); it != m_evt_handlers.end(); ++it )
    {
        ( **it )( ATTACHED, m_refbody );
    }
}

void InertBody::Detach( void )
{
    if( !m_refbody )
    {
        return;
    }

    dsreal world_scale = World::m_scale;

    // recup derniere transfo body auquel on s'attache
    Matrix mat_b;
    m_refbody->GetLastWorldTransformation( mat_b );
    
    DrawSpace::Utils::Matrix mat_a3 = /*m_lastworldtrans*/ m_lastlocalworldtrans * mat_b;


    // memoriser mat_a3, pour le reinjecter en transfo initiale pour le nouveau body
    btScalar kmat[16];    
    btTransform tf_a3;

    kmat[0] = mat_a3( 0, 0 );
    kmat[1] = mat_a3( 0, 1 );
    kmat[2] = mat_a3( 0, 2 );
    kmat[3] = mat_a3( 0, 3 );

    kmat[4] = mat_a3( 1, 0 );
    kmat[5] = mat_a3( 1, 1 );
    kmat[6] = mat_a3( 1, 2 );
    kmat[7] = mat_a3( 1, 3 );

    kmat[8] = mat_a3( 2, 0 );
    kmat[9] = mat_a3( 2, 1 );
    kmat[10] = mat_a3( 2, 2 );
    kmat[11] = mat_a3( 2, 3 );

    kmat[12] = mat_a3( 3, 0 ) * world_scale;
    kmat[13] = mat_a3( 3, 1 ) * world_scale;
    kmat[14] = mat_a3( 3, 2 ) * world_scale;
    kmat[15] = mat_a3( 3, 3 );

    tf_a3.setFromOpenGLMatrix( kmat );


    ///////////////////////////////////////////////////////

    btVector3  bt_linearspeed_mem;
    btVector3  bt_angularspeed_mem;

    bt_linearspeed_mem = m_rigidBody->getLinearVelocity();
    bt_angularspeed_mem = m_rigidBody->getAngularVelocity();

    ///////////////////////////////////////////////////////

    // detruire le body...
    destroy_body();

    // on revient dans le monde bullet global...
    m_world = m_global_world_mem;

    // recreer le body...
    create_body( tf_a3 );

    // bt_angularspeed_mem & bt_linearspeed_mem a passer dans le repere global...

    Vector angularspeed_mem, linearspeed_mem;
    Vector angularspeed_mem_2, linearspeed_mem_2;


    angularspeed_mem[0] = bt_angularspeed_mem.x();
    angularspeed_mem[1] = bt_angularspeed_mem.y();
    angularspeed_mem[2] = bt_angularspeed_mem.z();
    angularspeed_mem[3] = 1.0;


    linearspeed_mem[0] = bt_linearspeed_mem.x();
    linearspeed_mem[1] = bt_linearspeed_mem.y();
    linearspeed_mem[2] = bt_linearspeed_mem.z();
    linearspeed_mem[3] = 1.0;
  
    mat_b.ClearTranslation();
   

    mat_b.Transform( &angularspeed_mem, &angularspeed_mem_2 );
    mat_b.Transform( &linearspeed_mem, &linearspeed_mem_2 );
   
    m_rigidBody->setAngularVelocity( btVector3( angularspeed_mem_2[0], angularspeed_mem_2[1], angularspeed_mem_2[2] ) );
    m_rigidBody->setLinearVelocity( btVector3( linearspeed_mem_2[0], linearspeed_mem_2[1], linearspeed_mem_2[2] ) );

    //////////////////////////////////////

    m_refbody = NULL;

    for( std::vector<EventHandler*>::iterator it = m_evt_handlers.begin(); it != m_evt_handlers.end(); ++it )
    {
        ( **it )( DETACHED, NULL );
    }
}

void InertBody::GetLastLocalWorldTrans( DrawSpace::Utils::Matrix& p_mat )
{
    p_mat = m_lastlocalworldtrans;
}

void InertBody::ApplyForce( const DrawSpace::Utils::Vector p_force )
{
    dsreal world_scale = World::m_scale;

    m_rigidBody->applyForce( btVector3( p_force[0] * world_scale, 
                                p_force[1] * world_scale, 
                                p_force[2] * world_scale ), 
                                btVector3( 0.0, 0.0, 0.0 ) );
}

dsreal InertBody::GetLinearSpeedMagnitude( void )
{
    btVector3 speed = m_rigidBody->getLinearVelocity();
    Vector speed2( speed.x(), speed.y(), speed.z(), 1.0 );

    return speed2.Length() * World::m_scale;
}

dsreal InertBody::GetAngularSpeedMagnitude( void )
{
    btVector3 speed = m_rigidBody->getAngularVelocity();
    Vector speed2( speed.x(), speed.y(), speed.z(), 1.0 );

    return speed2.Length();
}

btRigidBody* InertBody::GetRigidBody( void )
{
    return m_rigidBody;
}

void InertBody::GetTotalForce( DrawSpace::Utils::Vector& p_force )
{
    btVector3 force = m_rigidBody->getTotalForce();

    p_force[0] = force.x();
    p_force[1] = force.y();
    p_force[2] = force.z();
    p_force[3] = 1.0;
}

void InertBody::GetTotalTorque( DrawSpace::Utils::Vector& p_torque )
{
    btVector3 torque = m_rigidBody->getTotalTorque();

    p_torque[0] = torque.x();
    p_torque[1] = torque.y();
    p_torque[2] = torque.z();
    p_torque[3] = 1.0;
}

void InertBody::RegisterEvtHandler( EventHandler* p_handler )
{
    m_evt_handlers.push_back( p_handler );

    Body::Event evt = ( NULL == m_refbody ? DETACHED : ATTACHED );
    (*p_handler)( evt, m_refbody );
}

Body* InertBody::GetRefBody( void )
{
    return m_refbody;
}