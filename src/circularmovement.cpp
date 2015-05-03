/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "circularmovement.h"
#include "maths.h"
#include "transformation.h"
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

CircularMovement::CircularMovement( void ) :
m_angular_speed( 0.0 )
{
}

CircularMovement::~CircularMovement( void )
{
}

void CircularMovement::Init( const Vector& p_center_pos, const Vector& p_delta_center, const Vector& p_rotaxis, dsreal p_init_angle, dsreal p_theta, dsreal p_phi )
{
    m_rotaxis = p_rotaxis;
    m_center_pos = p_center_pos;
    m_delta_center = p_delta_center;

    m_qyaw.Identity();
	m_qpitch.Identity();
	m_rot_res.Identity();

    m_init_angle = p_init_angle;

    Reset();
    SetTheta( p_theta );
    SetPhi( p_phi );
}

void CircularMovement::Compute( Utils::TimeManager& p_timemanager )
{
    Transformation transformation;

    /////////////////////////////////////////////////

    Matrix orientation;

	Vector yaxis( 0.0, 1.0, 0.0, 1.0 );
	Vector xaxis( 1.0, 0.0, 0.0, 1.0 );

	m_qyaw.RotationAxis( yaxis, m_current_theta );
	m_qpitch.RotationAxis( xaxis, m_current_phi );

	m_rot_res = m_qpitch * m_qyaw;
	m_rot_res.RotationMatFrom( orientation );

    /////////////////////////////////////////////////

    p_timemanager.AngleSpeedInc( &m_current_angle, m_angular_speed );

    dsreal final_angle = m_current_angle + m_init_angle;
    if( final_angle >= 360.0 )
    {
        final_angle -= 360.0;
    }
    else if( final_angle < 0.0 )
    {
        final_angle = 360.0 + final_angle;
    }


    Matrix delta_center;
    delta_center.Translation( m_delta_center );

    Matrix rotation;
    rotation.Rotation( m_rotaxis, DrawSpace::Utils::Maths::DegToRad( final_angle ) );

    Matrix position;
    position.Translation( m_center_pos );



    transformation.PushMatrix( position );
    transformation.PushMatrix( rotation );
    transformation.PushMatrix( delta_center );
    transformation.PushMatrix( orientation );
    transformation.BuildResult();
    transformation.GetResult( &m_result );    
}

void CircularMovement::SetAngularSpeed( dsreal p_angular_speed )
{
    m_angular_speed = p_angular_speed;
}

dsreal CircularMovement::GetAngularSpeed( void )
{
    return m_angular_speed;
}

void CircularMovement::SetTheta( dsreal p_theta )
{
    m_current_theta = DrawSpace::Utils::Maths::DegToRad( p_theta );
}

void CircularMovement::SetPhi( dsreal p_phi )
{
    m_current_phi = DrawSpace::Utils::Maths::DegToRad( p_phi );
}

void CircularMovement::Reset( void )
{
    m_current_angle = 0.0;
}

void CircularMovement::GetCenterPos( Utils::Vector& p_center_pos )
{
    p_center_pos = m_center_pos;
}

void CircularMovement::GetDeltaCenter( Utils::Vector& p_delta_center )
{
    p_delta_center = m_delta_center;
}

void CircularMovement::GetRotAxis( Utils::Vector& p_rotaxis )
{
    p_rotaxis = m_rotaxis;
}

dsreal CircularMovement::GetInitAngle( void )
{
    return m_init_angle;
}

dsreal CircularMovement::GetCurrentAngle( void )
{
    return m_current_angle;
}

dsreal CircularMovement::GetCurrentTheta( void )
{
    return Maths::RadToDeg( m_current_theta );
}

dsreal CircularMovement::GetCurrentPhi( void )
{
    return Maths::RadToDeg( m_current_phi );
}

void CircularMovement::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Compute( p_timemanager );
}

void CircularMovement::GetBaseTransform( Matrix& p_mat )
{
    p_mat = m_result;
}
