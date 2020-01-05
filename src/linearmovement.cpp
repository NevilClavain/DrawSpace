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

#include "linearmovement.h"
#include "maths.h"
#include "misc_utils.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


LinearMovement::LinearMovement( void ) :
m_speed( 0.0 )
{
}

LinearMovement::~LinearMovement( void )
{

}

void LinearMovement::Init( const Utils::Vector& p_init_pos, const Utils::Vector& p_direction, dsreal p_theta, dsreal p_phi )
{
    m_direction = p_direction;
    m_direction.Normalize();

    m_init_pos = p_init_pos;

    m_qyaw.Identity();
	m_qpitch.Identity();
	m_rot_res.Identity();

    Reset();
    SetTheta( p_theta );
    SetPhi( p_phi );
}

void LinearMovement::Compute( Utils::TimeManager& p_timemanager )
{
    /////////////////////////////////////////////////

    Matrix orientation;

	Vector yaxis( 0.0, 1.0, 0.0, 1.0 );
	Vector xaxis( 1.0, 0.0, 0.0, 1.0 );

	m_qyaw.RotationAxis( yaxis, m_current_theta );
	m_qpitch.RotationAxis( xaxis, m_current_phi );

	m_rot_res = m_qpitch * m_qyaw;
	m_rot_res.RotationMatFrom( orientation );

    /////////////////////////////////////////////////

    Matrix translation;

    dsreal delta_x = m_current_pos[0];
    dsreal delta_y = m_current_pos[1];
    dsreal delta_z = m_current_pos[2];

    p_timemanager.TranslationSpeedInc( &delta_x, m_speed * m_direction[0] );
    p_timemanager.TranslationSpeedInc( &delta_y, m_speed * m_direction[1] );
    p_timemanager.TranslationSpeedInc( &delta_z, m_speed * m_direction[2] );
    
    m_current_pos[0] = delta_x;
    m_current_pos[1] = delta_y;
    m_current_pos[2] = delta_z;

    translation.Translation( m_current_pos[0] + m_init_pos[0], m_current_pos[1] + m_init_pos[1], m_current_pos[2] + m_init_pos[2] );

    /////////////////////////////////////////////////

    m_result = orientation * translation;
}

void LinearMovement::SetSpeed( dsreal p_speed )
{
    m_speed = p_speed;
}

void LinearMovement::Reset( void )
{
    m_current_pos[0] = 0.0;
    m_current_pos[1] = 0.0;
    m_current_pos[2] = 0.0;
}

void LinearMovement::SetTheta( dsreal p_theta )
{
    m_current_theta = DrawSpace::Utils::Maths::DegToRad( p_theta );
}

void LinearMovement::SetPhi( dsreal p_phi )
{
    m_current_phi = Maths::DegToRad( p_phi );
}

dsreal LinearMovement::GetTranslationLength( void )
{
    return m_current_pos.Length();
}

void LinearMovement::GetInitPos( Utils::Vector& p_init_pos )
{
    p_init_pos = m_init_pos;
}

void LinearMovement::GetCurrentPos( Utils::Vector& p_current_pos )
{
    p_current_pos = m_current_pos;
}

void LinearMovement::GetDirection( Utils::Vector& p_direction )
{
    p_direction = m_direction;
}

dsreal LinearMovement::GetCurrentTheta( void )
{
    return Maths::RadToDeg( m_current_theta );
}

dsreal LinearMovement::GetCurrentPhi( void )
{
    return Maths::RadToDeg( m_current_phi );
}

dsreal LinearMovement::GetCurrentSpeed( void )
{
    return m_speed;
}

void LinearMovement::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Compute( p_timemanager );
}

void LinearMovement::GetBaseTransform( Matrix& p_mat )
{
    p_mat = m_result;
}
