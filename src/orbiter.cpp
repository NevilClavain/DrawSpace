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
#include "transformation.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


void Orbit::orbit_step( dsreal p_angle, Matrix& p_mat )
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

    Matrix orbit;
    orbit.Translation( x, 0.0, z );

    Matrix orbit2;
    orbit2.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( 360.0 - p_angle ) );

    Matrix tilt;
    tilt.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( m_tilt_angle ) );

    Matrix offset_rot;
    offset_rot.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( 360.0 - m_offset_angle ) );

    Matrix orb_trans;
    orb_trans.Translation( m_offset_plane_x, 0.0, m_offset_plane_y );

    transformation.PushMatrix( offset_rot );
    transformation.PushMatrix( tilt );
    transformation.PushMatrix( orb_trans );
    transformation.PushMatrix( orbit );
    transformation.PushMatrix( orbit2 );    

    transformation.BuildResult();
    transformation.GetResult( &result );

    p_mat = result;
}

void Orbit::OrbitStep( const Matrix& p_centroidbase )
{
    Matrix local_orbit;
    orbit_step( m_orbit_angle, local_orbit );

    if( m_drawable )
    {
        m_drawable->SetLocalTransform( p_centroidbase );
    }
    m_centroid->Update( p_centroidbase, local_orbit );
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
        orbit_step( i * p_anglestep, mat );
        
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

void Orbit::RegisterDrawable( DrawSpace::Interface::Drawable* p_drawable )
{
    m_drawable = p_drawable;
}

void Orbit::Progress( TimeManager& p_timer )
{
    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Orbiter::Orbiter( World* p_world, DrawSpace::Interface::Drawable* p_drawable ) : Body( p_world, p_drawable ),
m_rigidBody( NULL ),
m_collisionShape( NULL ),
m_motionState( NULL )
{
}

Orbiter::~Orbiter( void )
{
}

void Orbiter::Update( const Matrix& p_mat )
{
    m_lastworldtrans = p_mat;

    Matrix orbiter_transform = p_mat;
    m_drawable->SetLocalTransform( orbiter_transform );
}

/* a supprimer ?? */
void Orbiter::SetKinematic( const Body::Parameters& p_parameters )
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
}

/* a supprimer ?? */
void Orbiter::UnsetKinematic( void )
{
    m_world->getBulletWorld()->removeRigidBody( m_rigidBody );
    _DRAWSPACE_DELETE_( m_rigidBody );
    _DRAWSPACE_DELETE_( m_collisionShape );
    _DRAWSPACE_DELETE_( m_motionState );
}

////////////////////////////////////////////////////////////////////////////

Centroid::Centroid( void ) : m_orbiter( NULL )
{
}

void Centroid::RegisterSubOrbit( Orbit* p_orbit )
{
    m_sub_orbits.push_back( p_orbit );
}

void Centroid::Update( const Matrix& p_prevcentroidbase, const Matrix& p_localorbitmat )
{   
    m_transformation = p_localorbitmat * p_prevcentroidbase;
    for( size_t i = 0; i < m_sub_orbits.size(); i++ )
    {       
        m_sub_orbits[i]->OrbitStep( m_transformation );
    }
    if( m_orbiter )
    {
        m_orbiter->Update( m_transformation );
    }
}

void Centroid::SetOrbiter( Orbiter* p_orbiter )
{
    m_orbiter = p_orbiter;
}


