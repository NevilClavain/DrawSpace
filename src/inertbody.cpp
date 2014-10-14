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
#include "exceptions.h"
#include "configsbase.h"
#include "assetsbase.h"
#include "misc_utils.h"
#include "transformation.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


InertBody::InertBody( void ) :
m_refbody( NULL ),
m_rigidBody( NULL ),
m_collisionShape( NULL ),
m_motionState( NULL ),
m_meshe_data( NULL ),
m_drawable( NULL ),
m_global_world_mem( NULL )
{
    init();
}

InertBody::InertBody( World* p_world, TransformNode* p_drawable, const Body::Parameters& p_parameters ) : Body( p_world ),
m_refbody( NULL ),
m_parameters( p_parameters ),
m_rigidBody( NULL ),
m_collisionShape( NULL ),
m_motionState( NULL ),
m_meshe_data( NULL ),
m_drawable( p_drawable )
{
    m_global_world_mem = m_world;

    init();
    init_body();
}

InertBody::~InertBody( void )
{
    destroy_body();
}

void InertBody::init( void )
{
    // properties array creation
    m_properties["mass"].AddPropValue<dsreal>( 1.0 );
    m_properties["initial_attitude"].AddProp<std::vector<BodyInitialAttitudComponent>>();
    m_properties["shape_type"].AddProp<Shape>();
    m_properties["sphere_shape_radius"].AddProp<dsreal>();
    m_properties["box_shape_dims"].AddProp<Vector>();
    m_properties["meshe_shape_name"].AddProp<dsstring>();

    m_lastlocalworldtrans.Identity();
}

void InertBody::init_body( void )
{
    btScalar    btmat[16];
    btTransform bt_transform;

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

void InertBody::SetWorld( World* p_world )
{
    m_world             = p_world;
    m_global_world_mem  = m_world;
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

TransformNode* InertBody::GetDrawable( void )
{
    return m_drawable;
}

void InertBody::SetDrawable( DrawSpace::Core::TransformNode* p_drawable )
{
    m_drawable = p_drawable;
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

void InertBody::GetLinearSpeed( DrawSpace::Utils::Vector& p_speed )
{
    btVector3 speed = m_rigidBody->getLinearVelocity();

    p_speed[0] = speed.x();
    p_speed[1] = speed.y();
    p_speed[2] = speed.z();
    p_speed[3] = 1.0;
}

dsreal InertBody::GetBoundingSphereRay( void )
{
    btVector3 pos;
    btScalar  ray;
    m_collisionShape->getBoundingSphere( pos, ray );

    return ray;
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

bool InertBody::HasLanded( void )
{
    btVector3 lv = m_rigidBody->getLinearVelocity();
    btVector3 av = m_rigidBody->getAngularVelocity();

    bool status = GetContactState() && lv.length() < 0.001 && av.length() < 0.001;

    // PROVISOIRE, pour tests
    if( status )
    {
        m_rigidBody->activate( false );
    }
    else
    {
        m_rigidBody->activate( true );
    }

    return status;
}

bool InertBody::IsActive( void )
{
    return m_rigidBody->isActive();
}

bool InertBody::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "mass" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["mass"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else if( "initial_attitude" == p_words[0] )
    {
        BodyInitialAttitudComponent att;

        std::vector<BodyInitialAttitudComponent> initial_attitude = m_properties["initial_attitude"].GetPropValue<std::vector<BodyInitialAttitudComponent>>();

        if( p_words.size() < 5 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        Vector v;
        for( long i = 0; i < 3; i++ )
        {
            v[i] = StringToReal( p_words[i + 2] );
        }
        v[3] = 1.0;

        att.vector = v;

        m_properties["head_pos"].SetPropValue<Vector>( v );

        if( "translation" == p_words[1] )
        {
            att.type = BODY_INITIAL_ATTITUDE_TRANSLATION;
        }
        else if( "rotation" == p_words[1] )
        {
            att.type = BODY_INITIAL_ATTITUDE_ROTATION;

            if( p_words.size() < 6 )
            {
                _PARSER_MISSING_ARG__
                return false;
            }
            att.angle = StringToReal( p_words[5] );
        }
        else
        {
            _PARSER_UNEXPECTED_KEYWORD_
        }

        initial_attitude.push_back( att );

        m_properties["initial_attitude"].SetPropValue<std::vector<BodyInitialAttitudComponent>>( initial_attitude );
    }
    else if( "shape_type" == p_words[0] )
    {
        if( p_words.size() < 3 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        if( "meshe_shape" == p_words[1] )
        {
            m_properties["shape_type"].SetPropValue<Shape>( MESHE_SHAPE );
            m_properties["meshe_shape_name"].SetPropValue<dsstring>( p_words[2] );
        }
        else if( "sphere_shape " == p_words[1] )
        {
            m_properties["shape_type"].SetPropValue<Shape>( SPHERE_SHAPE );
            m_properties["sphere_shape_radius"].SetPropValue<dsreal>( StringToReal( p_words[2] ) );
        }
        else if( "box_shape " == p_words[1] )
        {
            m_properties["shape_type"].SetPropValue<Shape>( BOX_SHAPE );

            if( p_words.size() < 5 )
            {
                _PARSER_MISSING_ARG__
                return false;
            }

            Vector v;
            for( long i = 0; i < 3; i++ )
            {
                v[i] = StringToReal( p_words[i + 2] );
            }
            v[3] = 1.0;

            m_properties["box_shape_dims"].SetPropValue<Vector>( v );
        }
        else
        {
            _PARSER_UNEXPECTED_KEYWORD_
        }        
    }

    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void InertBody::Serialize( Utils::Archive& p_archive  )
{
}

bool InertBody::Unserialize( Utils::Archive& p_archive )
{
    return true;
}

void InertBody::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    p_text += "mass ";
    RealToString( m_properties["mass"].GetPropValue<dsreal>(), text_value );
    p_text += text_value;
    p_text += "\r\n";

    std::vector<BodyInitialAttitudComponent> initial_attitude = m_properties["initial_attitude"].GetPropValue<std::vector<BodyInitialAttitudComponent>>();
    for( size_t i = 0; i < initial_attitude.size(); i++ )
    {
        p_text += "initial_attitude ";
            
        BodyInitialAttitudComponent curr = initial_attitude[i];

        if( BODY_INITIAL_ATTITUDE_ROTATION == curr.type )
        {
            p_text += "translation ";

            for( long i = 0; i < 3; i++ )
            {
                RealToString( curr.vector[i], text_value );
                p_text += text_value;
                p_text += " ";
            }            
        }
        else // BODY_INITIAL_ATTITUDE_TRANSLATION
        {
            p_text += "rotation ";

            for( long i = 0; i < 3; i++ )
            {
                RealToString( curr.vector[i], text_value );
                p_text += text_value;
                p_text += " ";
            }
            RealToString( curr.angle, text_value );
        }
        p_text += "\r\n";
    }

    p_text += "shape_type ";

    switch( m_properties["shape_type"].GetPropValue<Shape>() )
    {
        case BOX_SHAPE:
            {
                p_text += "box_shape ";

                Vector box_shape_dims = m_properties["box_shape_dims"].GetPropValue<Vector>();
                for( long i = 0; i < 3; i++ )
                {
                    RealToString( box_shape_dims[i], text_value );

                    p_text += text_value;
                    p_text += " ";
                }
                p_text += "\r\n";
            }
            break;

        case SPHERE_SHAPE:

            p_text += "sphere_shape ";

            RealToString( m_properties["sphere_shape_radius"].GetPropValue<dsreal>(), text_value );
            p_text += text_value;
            p_text += "\r\n";

            break;

        case MESHE_SHAPE:

            p_text += "meshe_shape ";
            p_text += m_properties["meshe_shape_name"].GetPropValue<dsstring>();
            p_text += "\r\n";

            break;
    }

    p_text += "\r\n";
}

bool InertBody::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void InertBody::ApplyProperties( void )
{
    m_parameters.mass = m_properties["mass"].GetPropValue<dsreal>();

    Transformation transf;
    std::vector<BodyInitialAttitudComponent> initial_attitude = m_properties["initial_attitude"].GetPropValue<std::vector<BodyInitialAttitudComponent>>();
    for( size_t i = 0; i < initial_attitude.size(); i++ )
    {
        BodyInitialAttitudComponent curr = initial_attitude[i];

        Matrix mat;

        if( BODY_INITIAL_ATTITUDE_ROTATION == curr.type )
        {
            mat.Translation( curr.vector );
        }
        else // BODY_INITIAL_ATTITUDE_TRANSLATION
        {
            mat.Rotation( curr.vector, curr.angle );
        }

        transf.PushMatrix( mat );
    }

    transf.BuildResult();
    transf.GetResult( &m_parameters.initial_attitude );

    m_parameters.shape_descr.shape = m_properties["shape_type"].GetPropValue<Shape>();

    switch( m_parameters.shape_descr.shape )
    {
        case BOX_SHAPE:

            m_parameters.shape_descr.box_dims = m_properties["box_shape_dims"].GetPropValue<Vector>();
            break;

        case SPHERE_SHAPE:

            m_parameters.shape_descr.sphere_radius = m_properties["sphere_shape_radius"].GetPropValue<dsreal>();
            break;

        case MESHE_SHAPE:
            {
                dsstring meshe_name = m_properties["meshe_shape_name"].GetPropValue<dsstring>();

                if( false == AssetsBase::GetInstance()->AssetIdExists( meshe_name ) )
                {
                    _DSEXCEPTION( "Meshe Asset id unknown in AssetsBase" );
                }
                
                Asset* asset = AssetsBase::GetInstance()->GetAsset( meshe_name );

                Meshe* meshe = dynamic_cast<Meshe*>( asset );
                if( !meshe )
                {
                    _DSEXCEPTION( "Specified asset is not a Meshe" );
                }

                m_parameters.shape_descr.meshe = *meshe;
            }
            break;
    }

    init_body();
}

Configurable* InertBody::Instanciate( void )
{
    return _DRAWSPACE_NEW_( InertBody, InertBody );
}

void InertBody::GetKeyword( dsstring& p_outkeyword )
{
    p_outkeyword = INERTBODY_TEXT_KEYWORD;
}