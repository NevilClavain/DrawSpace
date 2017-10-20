/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "bodyaspect.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;


BodyAspect::BodyAspect( void ) :
m_motionState( NULL ),
m_collisionShape( NULL ),
m_rigidBody( NULL ),
m_mesh( NULL ),
m_tr_aspectimpl( &m_motionState ),
m_body_active( true )
{
}

BodyAspect::~BodyAspect( void )
{
}

btRigidBody* BodyAspect::GetRigidBody( void ) const
{
    return m_rigidBody;
}

btRigidBody* BodyAspect::Init( void )
{
    ///////////////////////////////////////

    ComponentList<bool> flags;
    GetComponentsByType<bool>( flags );
    bool collider = false;

    if( flags.size() > 0 )
    {    
        collider = flags[0]->getPurpose();
    }


    ComponentList<Matrix> mats;     
    GetComponentsByType<Matrix>( mats );

    Matrix attitude_mat;
    if( mats.size() )
    {
        attitude_mat = mats[0]->getPurpose();
    }
    else
    {
        attitude_mat.Identity();
    }

    /*
    ComponentList<Shape> shapes;
    GetComponentsByType<Shape>( shapes );

    Shape shape = shapes[0]->getPurpose();
    */

    ComponentList<dsreal> reals;
    GetComponentsByType<dsreal>( reals );

    dsreal mass = 0.0;
    
    if( reals.size() > 0 && !collider )
    {
        mass = reals[0]->getPurpose();
    }

    ///////////////////////////////////////


    btScalar    btmat[16];
    btTransform bt_transform;

    btmat[0] = attitude_mat( 0, 0 );
    btmat[1] = attitude_mat( 0, 1 );
    btmat[2] = attitude_mat( 0, 2 );
    btmat[3] = attitude_mat( 0, 3 );

    btmat[4] = attitude_mat( 1, 0 );
    btmat[5] = attitude_mat( 1, 1 );
    btmat[6] = attitude_mat( 1, 2 );
    btmat[7] = attitude_mat( 1, 3 );

    btmat[8] = attitude_mat( 2, 0 );
    btmat[9] = attitude_mat( 2, 1 );
    btmat[10] = attitude_mat( 2, 2 );
    btmat[11] = attitude_mat( 2, 3 );

    btmat[12] = attitude_mat( 3, 0 );
    btmat[13] = attitude_mat( 3, 1 );
    btmat[14] = attitude_mat( 3, 2 );
    btmat[15] = attitude_mat( 3, 3 );


    bt_transform.setFromOpenGLMatrix( btmat );

    //////////////////////////////////////////////////////

    ComponentList<BoxCollisionShape> boxcollision_shapes;
    ComponentList<SphereCollisionShape> spherecollision_shapes;
    ComponentList<MesheCollisionShape> meshecollision_shapes;

    GetComponentsByType<BoxCollisionShape>( boxcollision_shapes );
    GetComponentsByType<SphereCollisionShape>( spherecollision_shapes );
    GetComponentsByType<MesheCollisionShape>( meshecollision_shapes );

    if( boxcollision_shapes.size() )
    {
        DrawSpace::Utils::Vector box_dims;
        box_dims = boxcollision_shapes[0]->getPurpose().m_box;

        btBoxShape* shape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( box_dims[0], box_dims[1], box_dims[2] ) ) );
        m_collisionShape = shape;
    }
    else if( spherecollision_shapes.size() )
    {
        dsreal sphere_radius = spherecollision_shapes[0]->getPurpose().m_ray;
        m_collisionShape = _DRAWSPACE_NEW_( btSphereShape, btSphereShape( sphere_radius ) );
    }
    else if( meshecollision_shapes.size() )
    {
        Meshe meshe = meshecollision_shapes[0]->getPurpose().m_meshe;

        m_mesh = _DRAWSPACE_NEW_( btTriangleMesh, btTriangleMesh );

        for( long i = 0; i < meshe.GetTrianglesListSize(); i++ )
        {
            Triangle curr_triangle;
            meshe.GetTriangles( i, curr_triangle );

            Vertex v1, v2, v3;

            meshe.GetVertex( curr_triangle.vertex1, v1 );
            meshe.GetVertex( curr_triangle.vertex2, v2 );
            meshe.GetVertex( curr_triangle.vertex3, v3 );

            btVector3 a( v1.x, v1.y, v1.z );
            btVector3 b( v2.x, v2.y, v2.z );
            btVector3 c( v3.x, v3.y, v3.z );

            m_mesh->addTriangle( a, b, c, false );
        }

        m_collisionShape = _DRAWSPACE_NEW_( btBvhTriangleMeshShape, btBvhTriangleMeshShape( m_mesh, true, true ) );   
    }

    ///////////////////////////////////////////////////////////////////////////

    m_motionState = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( bt_transform ) );

    btVector3 localInertia( 0, 0, 0 );

    if( mass > 0.0 && !collider )
    {        
        m_collisionShape->calculateLocalInertia( mass, localInertia );
    }

    //si collider, mass == 0.0
    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo( mass, m_motionState, m_collisionShape, localInertia );

    m_rigidBody = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( boxRigidBodyConstructionInfo ) );

    if( collider )
    {
        m_rigidBody->setCollisionFlags( m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
    }
    m_rigidBody->setActivationState( DISABLE_DEACTIVATION );

    return m_rigidBody;
}

void BodyAspect::Release( void )
{
    _DRAWSPACE_DELETE_( m_motionState );
    _DRAWSPACE_DELETE_( m_collisionShape );
    _DRAWSPACE_DELETE_( m_rigidBody );

    if( m_mesh )
    {
        _DRAWSPACE_DELETE_( m_mesh );
    }
}

AspectImplementations::BodyTransformAspectImpl* BodyAspect::GetTransformAspectImpl( void )
{
    return &m_tr_aspectimpl;
}

void BodyAspect::Update( void )
{
    ComponentList<bool> flags;
    GetComponentsByType<bool>( flags );

    bool collider = false;
    if( flags.size() > 0 )
    {    
        collider = flags[0]->getPurpose();
    }

    btScalar                 bt_matrix[16];
    DrawSpace::Utils::Matrix local_transf;

    if( m_motionState && !collider )
    {
        m_motionState->m_graphicsWorldTrans.getOpenGLMatrix( bt_matrix );
   
        local_transf( 0, 0 ) = bt_matrix[0];
        local_transf( 0, 1 ) = bt_matrix[1];
        local_transf( 0, 2 ) = bt_matrix[2];
        local_transf( 0, 3 ) = bt_matrix[3];

        local_transf( 1, 0 ) = bt_matrix[4];
        local_transf( 1, 1 ) = bt_matrix[5];
        local_transf( 1, 2 ) = bt_matrix[6];
        local_transf( 1, 3 ) = bt_matrix[7];

        local_transf( 2, 0 ) = bt_matrix[8];
        local_transf( 2, 1 ) = bt_matrix[9];
        local_transf( 2, 2 ) = bt_matrix[10];
        local_transf( 2, 3 ) = bt_matrix[11];

        local_transf( 3, 0 ) = bt_matrix[12];
        local_transf( 3, 1 ) = bt_matrix[13];
        local_transf( 3, 2 ) = bt_matrix[14];
        local_transf( 3, 3 ) = bt_matrix[15];
    }

    
    //////////////////////////////////////////////
    // activer/desactiver cinematique du body (si pas collider)

    if( flags.size() > 1 && !collider )
    {
        bool enable_body = flags[1]->getPurpose();

        if( enable_body != m_body_active )
        {
            body_state( enable_body );            
        }
    }

    ///////////////////////////////////////////////
    // application des forces & torques

    if( m_motionState && !collider )
    {
        ComponentList<Force> forces;
        GetComponentsByType<Force>( forces );

        for( size_t i = 0; i < forces.size(); ++i )
        {
            Force applied_force = forces[i]->getPurpose();

            if( applied_force.m_enabled )
            {
                Vector force_v = applied_force.m_force_dir;
                force_v.Scale( applied_force.m_force_scale );

                Vector final_force_v;
        
                if( Force::LOCALE == applied_force.m_mode )
                {
                    Matrix local_rot = local_transf;
                    local_rot.ClearTranslation();
                    local_rot.Transform( &force_v, &final_force_v );            
                }
                else
                {
                    final_force_v = force_v;
                }

                m_rigidBody->applyForce( btVector3( final_force_v[0], final_force_v[1], final_force_v[2] ), 
                                            btVector3( 0.0, 0.0, 0.0 ) );
            }
        }

        ComponentList<Torque> torques;
        GetComponentsByType<Torque>( torques );

        for( size_t i = 0; i < torques.size(); ++i )
        {
            Torque applied_torque = torques[i]->getPurpose();

            if( applied_torque.m_enabled )
            {
                Vector torque_v = applied_torque.m_torque_axis;
                torque_v.Scale( applied_torque.m_torque_scale );

                Vector final_torque_v;
        
                if( Torque::LOCALE == applied_torque.m_mode )
                {
                    Matrix local_rot = local_transf;
                    local_rot.ClearTranslation();
                    local_rot.Transform( &torque_v, &final_torque_v );            
                }
                else
                {
                    final_torque_v = torque_v;
                }

                m_rigidBody->applyTorque( btVector3( final_torque_v[0], final_torque_v[1], final_torque_v[2] ) );
            }
        }
    }

    ///////////////////////////////////////////////
    // update du composant matrice index 0 ("attitude"), si ce composant existe bien sur :)

    ComponentList<Matrix> mats;     
    GetComponentsByType<Matrix>( mats );

    Matrix attitude_mat;
    if( mats.size() )
    {
        if( m_motionState && !collider )
        {
            // updater le composant matrice 'attitude'
            mats[0]->getPurpose() = local_transf;
        }
    }
}

void BodyAspect::body_state( bool p_enabled )
{
    if( m_rigidBody )
    {
        if( p_enabled )
        {
            m_rigidBody->activate();
        }
        else
        {
            m_rigidBody->forceActivationState( WANTS_DEACTIVATION );
        }

        m_body_active = p_enabled;
    }
}