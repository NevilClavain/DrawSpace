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

#include "freemovement.h"
#include "misc_utils.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

FreeMovement::FreeMovement( void )
{
}

FreeMovement::~FreeMovement( void )
{
}

void FreeMovement::Init( const Utils::Vector& p_init_pos )
{
	// init du quaternion resultat
	m_current_res.Identity();

	// init des axes tournants	
	m_current_x_axis[0] = 1.0;
	m_current_x_axis[1] = 0.0;
	m_current_x_axis[2] = 0.0;
	m_current_x_axis[3] = 1.0;

	m_current_y_axis[0] = 0.0;
	m_current_y_axis[1] = 1.0;
	m_current_y_axis[2] = 0.0;
	m_current_y_axis[3] = 1.0;

	m_current_z_axis[0] = 0.0;
	m_current_z_axis[1] = 0.0;
	m_current_z_axis[2] = 1.0;
	m_current_z_axis[3] = 1.0;
		
	// init des matrices resultats
    m_position.Translation( p_init_pos );
	m_orientation.Identity();
}

void FreeMovement::RotateYaw( dsreal p_rspeed, TimeManager& p_timemanager )
{
	Quaternion q, qres;

	dsreal fps = p_timemanager.ConvertUnitPerSecFramePerSec( p_rspeed );
	
	q.RotationAxis( m_current_y_axis, fps );
	qres = m_current_res * q;
	m_current_res = qres;

	// maj de la matrice resultat
	m_current_res.RotationMatFrom( m_orientation );

	// maj de l'axe
	m_current_y_axis[0] = m_orientation( 1, 0 );
	m_current_y_axis[1] = m_orientation( 1, 1 );
	m_current_y_axis[2] = m_orientation( 1, 2 );
}

void FreeMovement::RotatePitch( dsreal p_rspeed, TimeManager& p_timemanager )
{
	Quaternion q, qres;

	dsreal fps = p_timemanager.ConvertUnitPerSecFramePerSec( p_rspeed );

	q.RotationAxis( m_current_x_axis, fps );
	qres = m_current_res * q;
	m_current_res = qres;

	// maj de la matrice resultat
	m_current_res.RotationMatFrom( m_orientation );

	// maj de l'axe
	m_current_x_axis[0] = m_orientation( 0, 0 );
	m_current_x_axis[1] = m_orientation( 0, 1 );
	m_current_x_axis[2] = m_orientation( 0, 2 );
}

void FreeMovement::RotateRoll( dsreal p_rspeed, TimeManager& p_timemanager )
{
	Quaternion q, qres;

	dsreal fps = p_timemanager.ConvertUnitPerSecFramePerSec( p_rspeed );

	q.RotationAxis( m_current_z_axis, fps );
	qres = m_current_res * q;
	m_current_res = qres;

	// maj de la matrice resultat
	m_current_res.RotationMatFrom( m_orientation );

	// maj de l'axe
	m_current_z_axis[0] = m_orientation( 2, 0 );
	m_current_z_axis[1] = m_orientation( 2, 1 );
	m_current_z_axis[2] = m_orientation( 2, 2 );
}

void FreeMovement::RotateAxis( Utils::Vector& p_axis, dsreal p_rspeed, TimeManager& p_timemanager )
{
	Quaternion q, qres;

	dsreal fps = p_timemanager.ConvertUnitPerSecFramePerSec( p_rspeed );

	q.RotationAxis( p_axis, fps );
	qres = m_current_res * q;
	m_current_res = qres;

	// maj de la matrice resultat
	m_current_res.RotationMatFrom( m_orientation );	
}

void FreeMovement::SetSpeed( dsreal p_speed )
{
    m_local_speed[2] = -p_speed;
}

void FreeMovement::Compute( Utils::TimeManager& p_timemanager )
{
	Vector gs;
	m_orientation.Transform( &m_local_speed, &gs );

	dsreal x, y, z;
	x = m_position( 3, 0 );
	y = m_position( 3, 1 );
	z = m_position( 3, 2 );

	p_timemanager.TranslationSpeedInc( &x, gs[0] );
	p_timemanager.TranslationSpeedInc( &y, gs[1] );
	p_timemanager.TranslationSpeedInc( &z, gs[2] );

	m_position( 3, 0 ) = x;
	m_position( 3, 1 ) = y;
	m_position( 3, 2 ) = z;   

    m_result = m_orientation * m_position;
}

void FreeMovement::GetCurrentPos( Utils::Vector& p_pos )
{
    p_pos[0] = m_position( 3, 0 );
    p_pos[1] = m_position( 3, 1 );
    p_pos[2] = m_position( 3, 2 );
    p_pos[3] = 1.0;
}

