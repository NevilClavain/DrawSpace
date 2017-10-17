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


    ComponentList<Shape> shapes;
    GetComponentsByType<Shape>( shapes );

    Shape shape = shapes[0]->getPurpose();


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

    switch( shape )
    {
        case BOX_SHAPE:
            {
                ComponentList<Vector> vecs;
                GetComponentsByType<Vector>( vecs );
                DrawSpace::Utils::Vector    box_dims;
                box_dims = vecs[0]->getPurpose();

                btBoxShape* shape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( box_dims[0], box_dims[1], box_dims[2] ) ) );
                m_collisionShape = shape;
            }
            break;


        case SPHERE_SHAPE:
            {
                dsreal sphere_radius = reals[1]->getPurpose();

                m_collisionShape = _DRAWSPACE_NEW_( btSphereShape, btSphereShape( sphere_radius ) );
            }
            break;


        case MESHE_SHAPE:
            {
                ComponentList<Meshe> meshes;
                GetComponentsByType<Meshe>( meshes );

                Meshe meshe = meshes[0]->getPurpose();

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
            break;    
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
    

    //////////////////////////////////////////////

    if( flags.size() > 1 )
    {
        bool enable_body = flags[1]->getPurpose();

        if( enable_body != m_body_active )
        {
            body_state( enable_body );            
        }
    }

    ///////////////////////////////////////////////

    // update du composant matrice index 0 ("attitude"), si ce composant existe bien sur :)

    ComponentList<Matrix> mats;     
    GetComponentsByType<Matrix>( mats );

    Matrix attitude_mat;
    if( mats.size() )
    {
        btScalar                 bt_matrix[16];
        DrawSpace::Utils::Matrix updated_matrix;

        if( m_motionState && !collider )
        {
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

            // updater le composant matrice 'attitude'
            mats[0]->getPurpose() = updated_matrix;
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