/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include "orbiter.h"
#include "maths.h"
#include "transformation.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


void Orbit::orbit_step( dsreal p_angle, DrawSpace::Utils::Matrix& p_orbit_mat, DrawSpace::Utils::Matrix& p_planet_mat )
{
    dsreal a = 1.0;
    dsreal b = m_excentricity;

    dsreal rad_ang = Maths::DegToRad( p_angle );
    dsreal x = a * cos( rad_ang );
    dsreal z = b * sin( rad_ang );

    x = ( x * m_ray );
    z = ( z * m_ray );

    Transformation transformation;    
    Matrix result;

    Matrix orbit_translation;
    orbit_translation.Translation( x, 0.0, z );

    Matrix orbit2;
    orbit2.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( 360.0 - p_angle ) );

    Matrix revolution;
    revolution.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( m_revolution_angle ) );


    Matrix tilt;
    tilt.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( m_tilt_angle ) );

    Matrix offset_rot;
    offset_rot.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( 360.0 - m_offset_angle ) );

    Matrix orbit_offset;
    orbit_offset.Translation( m_offset_plane_x, 0.0, m_offset_plane_y );


    Matrix revolution_tilt;
    revolution_tilt.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( m_revolution_tilt_angle ) );

    // PROVISOIRE
    if( m_centroid )
    {
        transformation.PushMatrix( offset_rot );
        transformation.PushMatrix( tilt );
        transformation.PushMatrix( orbit_offset );
        transformation.PushMatrix( orbit_translation );    
        transformation.PushMatrix( orbit2 );  
        

        transformation.BuildResult();
        transformation.GetResult( &result );

        p_orbit_mat = result;


        transformation.ClearAll();
        transformation.PushMatrix( offset_rot );
        transformation.PushMatrix( tilt );
        transformation.PushMatrix( orbit_offset );
        transformation.PushMatrix( orbit_translation );
        transformation.PushMatrix( revolution_tilt );
        transformation.PushMatrix( revolution );
        transformation.PushMatrix( orbit2 );
        
        transformation.BuildResult();
        transformation.GetResult( &result );

        p_planet_mat = result;
    }
    else
    {
        transformation.PushMatrix( offset_rot );
        transformation.PushMatrix( tilt );
        transformation.PushMatrix( orbit_offset );
        transformation.PushMatrix( orbit_translation );    
               
        transformation.BuildResult();
        transformation.GetResult( &result );

        p_orbit_mat = result;

        m_basetransform = result;
    }
}

void Orbit::OrbitStep( const Matrix& p_centroidbase )
{
    Matrix local_orbit;
    Matrix local_planet;
    orbit_step( m_orbit_angle, local_orbit, local_planet );

    m_centroid->Update( p_centroidbase, local_orbit, local_planet );
}


void Orbit::build_orbit_meshe( dsreal p_anglestep, DrawSpace::Core::Meshe* p_meshe )
{
    dsreal nb_steps = 360.0 / p_anglestep;

    Vertex center;
    center.x = center.y = center.z = 0.0;
    center.tu[0] = 0.0;

    p_meshe->AddVertex( Vertex( 0.0, 0.0, 0.0 ) );

    for( long i = 0; i < nb_steps; i++ )
    {       
        Matrix mat;
        Matrix local_planet;
        orbit_step( i * p_anglestep, mat, local_planet );
        
        Vector orbit_point( 0.0, 0.0, 0.0, 1.0 );
        Vector orbit_point_res;

        mat.Transform( &orbit_point, &orbit_point_res );

        Vertex v;
        v.x = orbit_point_res[0];
        v.y = orbit_point_res[1];
        v.z = orbit_point_res[2];

        v.tu[0] = 1.0;

        p_meshe->AddVertex( v );

        if( i < nb_steps - 1 )
        {
            p_meshe->AddTriangle( Triangle( i + 1, i + 2, 0 ) );
        }
        else
        {
            p_meshe->AddTriangle( Triangle( i + 1, 1, 0 ) );
        }
    }
}


void Orbit::BuildMeshe( dsreal p_anglestep, DrawSpace::Core::Meshe* p_meshe )
{
    build_orbit_meshe( p_anglestep, p_meshe );
}

void Orbit::RegisterChunk( DrawSpace::Chunk* p_drawable )
{
    m_drawable = p_drawable;
}

void Orbit::GetBaseTransform( DrawSpace::Utils::Matrix& p_mat )
{
    p_mat = m_basetransform;
}

void Orbit::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Matrix local_orbit;
    Matrix local_planet;
    orbit_step( m_orbit_angle, local_orbit, local_planet );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Orbiter::Orbiter( World* p_world ) : Body( p_world ),
m_rigidBody( NULL ),
m_collisionShape( NULL ),
m_motionState( NULL ),
m_meshe_data( NULL ),
m_orbit_duration( 0.0 ),
m_revolution_duration( 0.0 ),
m_revolution_tilt_angle( 0.0 ),
m_orbit_angle( 0.0 ),
m_revolution_angle( 0.0 ),
m_owner( NULL )
{
}

Orbiter::~Orbiter( void )
{
}

void Orbiter::SetKinematic( const Body::Parameters& p_parameters )
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


void Orbiter::UnsetKinematic( void )
{

    _DRAWSPACE_DELETE_( m_rigidBody );
    _DRAWSPACE_DELETE_( m_collisionShape );
    _DRAWSPACE_DELETE_( m_motionState );

    if( m_meshe_data )
    {
        _DRAWSPACE_DELETE_( m_meshe_data );
    }
}


void Orbiter::AddToWorld( void )
{
    //m_world->getBulletWorld()->addRigidBody( m_rigidBody );
    m_world->AddBody( this );
}

void Orbiter::RemoveFromWorld( void )
{
    //m_world->getBulletWorld()->removeRigidBody( m_rigidBody );
    m_world->RemoveBody( this );
}


btRigidBody* Orbiter::GetRigidBody( void )
{
    return m_rigidBody;
}


void Orbiter::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Matrix revolution_tilt;
    revolution_tilt.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( m_revolution_tilt_angle ) );

    Matrix orbit2;
    orbit2.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( 360.0 - m_orbit_angle ) );

    Matrix revolution;
    revolution.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( m_revolution_angle ) );

    Transformation transformation;
    Matrix result;


    transformation.ClearAll();
    transformation.PushMatrix( revolution_tilt );
    transformation.PushMatrix( revolution );
    transformation.PushMatrix( orbit2 );
    
    transformation.BuildResult();
    transformation.GetResult( &result );

    m_basetransform = result;
}

void Orbiter::GetBaseTransform( DrawSpace::Utils::Matrix& p_mat )
{
    p_mat = m_basetransform;
}

void Orbiter::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    m_owner = static_cast<SceneNode<Orbiter>*>( p_node );
}

void Orbiter::OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
}

//BaseSceneNode* Orbiter::GetOwner( void )
DrawSpace::Core::SceneNode<Orbiter>* Orbiter::GetOwner( void )
{
    return m_owner;
}

void Orbiter::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    m_lastworldtrans = p_mat;
}

////////////////////////////////////////////////////////////////////////////

Centroid::Centroid( void )
{
}

void Centroid::RegisterSubOrbit( Orbit* p_orbit )
{
    m_sub_orbits.push_back( p_orbit );
}

void Centroid::Update( const Matrix& p_prevcentroidbase, const Matrix& p_localorbitmat, const Matrix& p_localplanetmat )
{   
    m_transformation = p_localorbitmat * p_prevcentroidbase;
    Matrix transformation2 = p_localplanetmat * p_prevcentroidbase;

    for( size_t i = 0; i < m_sub_orbits.size(); i++ )
    {       
        m_sub_orbits[i]->OrbitStep( m_transformation );
    }
}


void Centroid::GetBaseTransform( DrawSpace::Utils::Matrix& p_mat )
{
    Matrix id;
    id.Identity();

    p_mat = id;
}

