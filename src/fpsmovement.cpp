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


#include "fpsmovement.h"
#include "misc_utils.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

FPSMovement::FPSMovement( bool p_ymvt ) : m_ymvt( p_ymvt )
{
}

FPSMovement::~FPSMovement( void )
{

}

void FPSMovement::Init( const Utils::Vector& p_init_pos, dsreal p_initial_yaw, dsreal p_initial_pitch )
{
	m_qyaw.Identity();
	m_qpitch.Identity();
	m_current_res.Identity();

	m_position.Translation( p_init_pos );
	m_orientation.Identity();

	m_ayaw = p_initial_yaw;
	m_apitch = p_initial_pitch;
}

void FPSMovement::InitRot( void )
{
	m_qyaw.Identity();
	m_qpitch.Identity();
	m_current_res.Identity();

	m_orientation.Identity();

	m_ayaw = 0.0;
	m_apitch = 0.0;
}

void FPSMovement::RotateYaw( dsreal p_speed, TimeManager& p_timemanager )
{
    p_timemanager.AngleSpeedInc( &m_ayaw, p_speed );
}

void FPSMovement::RotatePitch( dsreal p_speed, TimeManager& p_timemanager )
{
    p_timemanager.AngleSpeedInc( &m_apitch, p_speed );
}

void FPSMovement::SetSpeed( dsreal p_speed )
{
    m_local_speed[2] = -p_speed;
}

void FPSMovement::Compute( TimeManager& p_timemanager )
{
	Vector gs;

	Vector yaxis( 0.0, 1.0, 0.0, 1.0 );
	Vector xaxis( 1.0, 0.0, 0.0, 1.0 );

	m_qyaw.RotationAxis( yaxis, m_ayaw );
	m_qpitch.RotationAxis( xaxis, m_apitch );

	m_current_res = m_qpitch * m_qyaw;
	m_current_res.RotationMatFrom( m_orientation );

	m_orientation.Transform( &m_local_speed, &gs );

	p_timemanager.TranslationSpeedInc( &m_position( 3, 0 ), gs[0] );

    
	if( m_ymvt )
	{
		// prendre aussi en compte la composante en Y (la camera peut aussi evoluer "en hauteur")
		p_timemanager.TranslationSpeedInc( &m_position( 3, 1 ), gs[1] );
	}
    

	p_timemanager.TranslationSpeedInc( &m_position( 3, 2 ), gs[2] );

    m_result = m_orientation * m_position;
}

void FPSMovement::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Compute( p_timemanager );
}

void FPSMovement::GetCurrentPos( Utils::Vector& p_pos )
{
    p_pos[0] = m_position( 3, 0 );
    p_pos[1] = m_position( 3, 1 );
    p_pos[2] = m_position( 3, 2 );
    p_pos[3] = 1.0;
}

dsreal FPSMovement::GetCurrentYaw( void )
{
    return m_ayaw;
}

dsreal FPSMovement::GetCurrentPitch( void )
{
    return m_apitch;
}

void FPSMovement::GetBaseTransform( Matrix& p_mat )
{
    p_mat = m_result;
}
