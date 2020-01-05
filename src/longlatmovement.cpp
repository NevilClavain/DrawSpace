/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#include "longlatmovement.h"
#include "maths.h"
#include "transformation.h"
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


LongLatMovement::LongLatMovement( void )
{
}

LongLatMovement::~LongLatMovement( void )
{
}

void LongLatMovement::Init( dsreal p_init_longitud_theta, dsreal p_init_latitud_phi, dsreal p_init_alt, dsreal p_init_theta, dsreal p_init_phi )
{
    m_longitud_theta = DrawSpace::Utils::Maths::DegToRad( p_init_longitud_theta );
    m_latitud_phi = DrawSpace::Utils::Maths::DegToRad( p_init_latitud_phi );

    m_longitud_theta_base = 0.0;
    m_latitud_phi_base = 0.0;


    m_alt = p_init_alt;

    SetTheta( p_init_theta );
    SetPhi( p_init_phi );

}

void LongLatMovement::Compute( Utils::TimeManager& p_timemanager )
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

    Matrix init_rot;
    init_rot.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), PI / 2.0 );

    Matrix altitude;
    altitude.Translation( 0.0, 0.0, m_alt );

    Matrix longitud;
    longitud.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), m_longitud_theta );

    Matrix latitud;
    latitud.Rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), m_latitud_phi );

    Matrix longitud_b;
    longitud_b.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), m_longitud_theta_base );

    Matrix latitud_b;
    latitud_b.Rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), m_latitud_phi_base );

    transformation.PushMatrix( longitud_b );
    transformation.PushMatrix( latitud_b );
    transformation.PushMatrix( longitud );
    transformation.PushMatrix( latitud );
    transformation.PushMatrix( altitude );
    transformation.PushMatrix( init_rot );
    transformation.PushMatrix( orientation );
    transformation.BuildResult();
    transformation.GetResult( &m_result );
}

void LongLatMovement::SetTheta( dsreal p_theta )
{
    m_current_theta = DrawSpace::Utils::Maths::DegToRad( p_theta );
}

void LongLatMovement::SetPhi( dsreal p_phi )
{
    m_current_phi = DrawSpace::Utils::Maths::DegToRad( p_phi );
}

void LongLatMovement::SetLatitud( dsreal p_latitud )
{
    m_latitud_phi = DrawSpace::Utils::Maths::DegToRad( p_latitud );
}

void LongLatMovement::SetLongitud( dsreal p_longitud )
{
    m_longitud_theta = DrawSpace::Utils::Maths::DegToRad( p_longitud );
}

void LongLatMovement::SetLatitudBase( dsreal p_latitud )
{
    m_latitud_phi_base = DrawSpace::Utils::Maths::DegToRad( p_latitud );
}

void LongLatMovement::SetLongitudBase( dsreal p_longitud )
{
    m_longitud_theta_base = DrawSpace::Utils::Maths::DegToRad( p_longitud );
}


void LongLatMovement::SetAlt( dsreal p_altitud )
{
    m_alt = p_altitud;
}

dsreal LongLatMovement::GetCurrentLongitud( void )
{
    return DrawSpace::Utils::Maths::RadToDeg( m_longitud_theta );
}

dsreal LongLatMovement::GetCurrentLatitud( void )
{
    return DrawSpace::Utils::Maths::RadToDeg( m_latitud_phi );
}

dsreal LongLatMovement::GetCurrentAltitud( void )
{
    return m_alt;
}

dsreal LongLatMovement::GetCurrentTheta( void )
{
    return DrawSpace::Utils::Maths::RadToDeg( m_current_theta );
}

dsreal LongLatMovement::GetCurrentPhi( void )
{
    return DrawSpace::Utils::Maths::RadToDeg( m_current_phi );
}

void LongLatMovement::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Compute( p_timemanager );
}

void LongLatMovement::GetBaseTransform( Matrix& p_mat )
{
    p_mat = m_result;
}


void LongLatMovement::GetXYZ( DrawSpace::Utils::Vector& p_out )
{
    Utils::Vector in;

    in[0] = m_alt;
    in[1] = m_longitud_theta;
    in[2] = m_latitud_phi;

    Utils::Maths::SphericaltoCartesian( in, p_out );
}

