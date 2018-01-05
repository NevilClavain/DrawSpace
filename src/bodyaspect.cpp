/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */

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

#include "entity.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;


BodyAspect::BodyAspect( void ) :
m_motionState( NULL ),
m_collisionShape( NULL ),
m_rigidBody( NULL ),
m_mesh( NULL ),
m_tr_aspectimpl( this ),
m_body_active( true ),
m_attachment_owner( NULL ),
m_init_as_attached( false ),
m_init_as_detached( false ),
m_mode( NOT_READY ),
m_prev_attachment_owner( NULL )
{
    m_mem_transform.Identity();
    m_mem_linearspeed = _DRAWSPACE_NEW_( btVector3, btVector3( 0.0, 0.0, 0.0 ) );
    m_mem_angularspeed = _DRAWSPACE_NEW_( btVector3, btVector3( 0.0, 0.0, 0.0 ) );
}

BodyAspect::~BodyAspect( void )
{
}


void BodyAspect::convert_matrix_to_bt( const Matrix& p_mat, btScalar* bt_matrix )
{
    bt_matrix[0] = p_mat( 0, 0 );
    bt_matrix[1] = p_mat( 0, 1 );
    bt_matrix[2] = p_mat( 0, 2 );
    bt_matrix[3] = p_mat( 0, 3 );

    bt_matrix[4] = p_mat( 1, 0 );
    bt_matrix[5] = p_mat( 1, 1 );
    bt_matrix[6] = p_mat( 1, 2 );
    bt_matrix[7] = p_mat( 1, 3 );

    bt_matrix[8] = p_mat( 2, 0 );
    bt_matrix[9] = p_mat( 2, 1 );
    bt_matrix[10] = p_mat( 2, 2 );
    bt_matrix[11] = p_mat( 2, 3 );

    bt_matrix[12] = p_mat( 3, 0 );
    bt_matrix[13] = p_mat( 3, 1 );
    bt_matrix[14] = p_mat( 3, 2 );
    bt_matrix[15] = p_mat( 3, 3 );
}

void BodyAspect::convert_matrix_from_bt( btScalar* bt_matrix, Utils::Matrix& p_mat )
{
    p_mat( 0, 0 ) = bt_matrix[0];
    p_mat( 0, 1 ) = bt_matrix[1];
    p_mat( 0, 2 ) = bt_matrix[2];
    p_mat( 0, 3 ) = bt_matrix[3];

    p_mat( 1, 0 ) = bt_matrix[4];
    p_mat( 1, 1 ) = bt_matrix[5];
    p_mat( 1, 2 ) = bt_matrix[6];
    p_mat( 1, 3 ) = bt_matrix[7];

    p_mat( 2, 0 ) = bt_matrix[8];
    p_mat( 2, 1 ) = bt_matrix[9];
    p_mat( 2, 2 ) = bt_matrix[10];
    p_mat( 2, 3 ) = bt_matrix[11];

    p_mat( 3, 0 ) = bt_matrix[12];
    p_mat( 3, 1 ) = bt_matrix[13];
    p_mat( 3, 2 ) = bt_matrix[14];
    p_mat( 3, 3 ) = bt_matrix[15];
}

btRigidBody* BodyAspect::GetRigidBody( void ) const
{
    return m_rigidBody;
}

btRigidBody* BodyAspect::Init( void )
{
    ///////////////////////////////////////

    ComponentList<Mode> modes;
    GetComponentsByType<Mode>( modes );

    if( modes.size() > 0 )
    {
        m_mode = modes[0]->getPurpose();
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

    ComponentList<dsreal> reals;
    GetComponentsByType<dsreal>( reals );

    dsreal mass = 0.0;
    
    if( reals.size() > 0 && BODY == m_mode )
    {
        mass = reals[0]->getPurpose();
    }

    ///////////////////////////////////////

    btScalar    btmat[16];
    btTransform bt_transform;

    if( m_init_as_attached )
    {
        Matrix mat_b;
        //m_attachment_owner->GetLastTransform( mat_b );
        m_attachment_owner->compute_global_transf( mat_b );
        mat_b.Inverse();

        DrawSpace::Utils::Matrix mat_a2 = m_mem_transform * mat_b;

        convert_matrix_to_bt( mat_a2, btmat );
    }
    else if( m_init_as_detached )
    {
        Matrix mat_b;
        //m_prev_attachment_owner->GetLastTransform( mat_b );
        m_prev_attachment_owner->compute_global_transf( mat_b );
        DrawSpace::Utils::Matrix mat_a3 = m_mem_localbt_transform * mat_b;

        convert_matrix_to_bt( mat_a3, btmat );
    }
    else
    {
        convert_matrix_to_bt( attitude_mat, btmat );
    }

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

    if( mass > 0.0 && BODY == m_mode )
    {        
        m_collisionShape->calculateLocalInertia( mass, localInertia );
    }

    //si collider, mass == 0.0
    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo( mass, m_motionState, m_collisionShape, localInertia );

    m_rigidBody = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( boxRigidBodyConstructionInfo ) );

    if( COLLIDER == m_mode || ATTRACTOR_COLLIDER == m_mode )
    {
        m_rigidBody->setCollisionFlags( m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );        
    }
    m_rigidBody->setActivationState( DISABLE_DEACTIVATION );

    return m_rigidBody;
}

void BodyAspect::Release( void )
{
    //////////// memoriser differentes choses avant de nettoyer...////////////////

    if( BODY == m_mode )
    {
        GetLastTransform( m_mem_transform );
        
        if( m_motionState )
        {
            btScalar bt_matrix[16];
            m_motionState->m_graphicsWorldTrans.getOpenGLMatrix( bt_matrix );
            convert_matrix_from_bt( bt_matrix, m_mem_localbt_transform );
        }

        *m_mem_linearspeed = m_rigidBody->getLinearVelocity();
        *m_mem_angularspeed = m_rigidBody->getAngularVelocity();
        
    }
    ////////////////////////////////////////////////////////////

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

    btScalar                 bt_matrix[16];
    DrawSpace::Utils::Matrix local_transf;


    if( ( m_init_as_attached || m_init_as_detached ) && m_rigidBody )
    {
        Matrix mat_b;

        if( m_init_as_attached )
        {
            //m_attachment_owner->GetLastTransform( mat_b );
            m_attachment_owner->compute_global_transf( mat_b );
            mat_b.Inverse();
        }
        else if( m_init_as_detached )
        {
            //m_prev_attachment_owner->GetLastTransform( mat_b );
            m_prev_attachment_owner->compute_global_transf( mat_b );
        }
                       
        // restitution des vitesses angulaires et lineaires...
        Vector angularspeed_mem, linearspeed_mem;
        Vector angularspeed_mem_2, linearspeed_mem_2;

        angularspeed_mem[0] = m_mem_angularspeed->x();
        angularspeed_mem[1] = m_mem_angularspeed->y();
        angularspeed_mem[2] = m_mem_angularspeed->z();
        angularspeed_mem[3] = 1.0;

        linearspeed_mem[0] = m_mem_linearspeed->x();
        linearspeed_mem[1] = m_mem_linearspeed->y();
        linearspeed_mem[2] = m_mem_linearspeed->z();
        linearspeed_mem[3] = 1.0;

        mat_b.ClearTranslation();

        mat_b.Transform( &angularspeed_mem, &angularspeed_mem_2 );
        mat_b.Transform( &linearspeed_mem, &linearspeed_mem_2 );
   
        m_rigidBody->setAngularVelocity( btVector3( angularspeed_mem_2[0], angularspeed_mem_2[1], angularspeed_mem_2[2] ) );
        m_rigidBody->setLinearVelocity( btVector3( linearspeed_mem_2[0], linearspeed_mem_2[1], linearspeed_mem_2[2] ) );

        ///////////////

        m_init_as_attached = false;
        m_init_as_detached = false;
    }

    if( m_motionState && BODY == m_mode )
    {
        m_motionState->m_graphicsWorldTrans.getOpenGLMatrix( bt_matrix );
        convert_matrix_from_bt( bt_matrix, local_transf );
    }

    
    //////////////////////////////////////////////
    // activer/desactiver cinematique du body (si pas collider)

    if( flags.size() > 0 && BODY == m_mode )
    {
        bool enable_body = flags[0]->getPurpose();

        if( enable_body != m_body_active )
        {
            body_state( enable_body );            
        }
    }

    ///////////////////////////////////////////////
    // application des forces & torques

    if( m_motionState && BODY == m_mode )
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

    ComponentList<Matrix> mats;     
    GetComponentsByType<Matrix>( mats );

    if( mats.size() )
    {
        if( m_motionState && ( COLLIDER == m_mode || ATTRACTOR_COLLIDER == m_mode ))
        {
            m_collider_local_mat = mats[0]->getPurpose(); 

            btScalar    btmat[16];
            btTransform bt_transform;

            Matrix local_mat;

            if( ATTRACTOR_COLLIDER == m_mode )
            {
                local_mat.Identity();
            }
            else
            {
                local_mat = m_collider_local_mat;
            }

            convert_matrix_to_bt( local_mat, btmat );

            bt_transform.setFromOpenGLMatrix( btmat );

            m_motionState->m_graphicsWorldTrans = bt_transform;
        }
        else
        {
            if( m_motionState && BODY == m_mode )
            {
                // updater le composant matrice 'attitude'
                mats[0]->getPurpose() = local_transf;
            }
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

void BodyAspect::ManageAttachment( BodyAspect* p_owner )
{
    if( NULL == p_owner )  // plus de noeud body ancetre dans la hierarchie d'entites : on se "detache" si c'est pas deja fait
    {
        if( m_attachment_owner )
        {
            detach();
        }
    }
    else // un noeud body ancetre dans la hierarchie d'entites : on s'attache a celui-ci si c'est pas deja fait
    {
        if( m_attachment_owner )
        {
            if( p_owner != m_attachment_owner )
            {
                detach();
                attach_to( p_owner );
            }
        }
        else
        {
            attach_to( p_owner );
        }
    }
}

/*

mat_a => matrice du body a attacher (exemple : ship)
mat_b => matrice du body auxquel on s'attache (exemple : planete)

   1/ attachement :
    
        a: mat_a2 = mat_a * ( mat_b ^ -1 )
        b: detruire puis recreer le body en lui donnant mat_a2 comme matrice initiale


    2/ pendant la phase d'attachement :

        body attache : injecter dans le drawable (via SetLocalTransform()) la matrice transfo suivante :
                    
                        mat_body * mat_b

                        avec mat_body = la transfo du body calculee par bullet

                        (au tout debut de l'attachement, mat_body = mat_a2)



    3/ detachement

        a: mat_a3 = mat_body * mat_b
        b: detruire puis recreer le body en lui donnant mat_a3 comme matrice initiale


*/
void BodyAspect::attach_to( BodyAspect* body_aspect )
{
    
    if( NOT_READY == m_mode )
    {
        // pas encore pr�t, ignorer; on repassera ici au prochain cycle ( attach_to appel� cycliquement tant que m_attachment_owner == NULL : cf BodyAspect::ManageAttachment() )
        return;
    }
    
     // pour l'instant, seul les BODY peuvent �tre attach�/d�tach�s a un ATTRACTOR_COLLIDER

    if( m_mode != BODY )
    {
        _DSEXCEPTION( "Cannot attach : body must be in BODY mode" )
    }

    if( body_aspect->m_mode != ATTRACTOR_COLLIDER )
    {
        _DSEXCEPTION( "Cannot attach : target body must be in ATTRACTOR_COLLIDER mode" )
    }

    m_attachment_owner = body_aspect;
    m_init_as_attached = true; // pour le prochain passage dans Init()
}

void BodyAspect::detach( void )
{
    m_prev_attachment_owner = m_attachment_owner;
    m_attachment_owner = NULL;

    m_init_as_detached = true; // pour le prochain passage dans Init()
}

void BodyAspect::GetLastTransform( Utils::Matrix& p_mat )
{
    btScalar                 bt_matrix[16];
    DrawSpace::Utils::Matrix updated_matrix;

    if( m_motionState )
    {
        if( ATTRACTOR_COLLIDER == m_mode )
        {
            updated_matrix = m_collider_local_mat;
        }
        else
        {
            m_motionState->m_graphicsWorldTrans.getOpenGLMatrix( bt_matrix );

            convert_matrix_from_bt( bt_matrix, updated_matrix );
        }
    }
    else
    {
        updated_matrix.Identity();
    }
   
    if( NULL == m_attachment_owner )
    {
        p_mat = updated_matrix;
    }
    else
    {
        // attached : ajouter la transfo du body auquel on est attache
        DrawSpace::Utils::Matrix mat_b;
        //m_attachment_owner->GetLastTransform( mat_b );
        m_attachment_owner->compute_global_transf( mat_b );

        DrawSpace::Utils::Matrix res = updated_matrix * mat_b;

        p_mat = res;    
    }
    
}

void BodyAspect::SetAncestorsList( std::vector<Core::Entity*>& p_ancestors )
{
    m_ancestors = p_ancestors;
}

void BodyAspect::compute_global_transf( Utils::Matrix& p_result )
{
    Matrix loc;
    GetLastTransform( loc );

    p_result = loc;
        
    Entity* parent = NULL;

    for( auto it = m_ancestors.rbegin(); it != m_ancestors.rend(); ++it )
    {
        TransformAspect* transform_aspect = (*it)->GetAspect<TransformAspect>();

        if( transform_aspect )
        {
            transform_aspect->ComputeTransforms( parent, (*it) );
            parent = (*it);
        }
    }

    if( m_ancestors.size() )
    {
        TransformAspect* transform_aspect = m_ancestors[0]->GetAspect<TransformAspect>();

        if( transform_aspect )
        {
            Matrix finalmat;
            transform_aspect->GetWorldTransform( finalmat );

            Matrix res;
            
            res = p_result * finalmat;
            p_result = res;
        }
    }

}
