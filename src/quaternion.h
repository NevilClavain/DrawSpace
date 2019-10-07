/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "matrix.h"

namespace DrawSpace
{
namespace Utils
{
class Quaternion
{
protected:
    dsreal      m_quat[4];

public:
    Quaternion(void);
	Quaternion(dsreal p_x, dsreal p_y, dsreal p_z, dsreal p_w);

    virtual ~Quaternion(void);

	inline dsreal operator[]( int p_index ) const
	{
		return m_quat[p_index];
	};

	inline dsreal& operator[]( int p_index )
	{
		return m_quat[p_index];
	};

	inline void Zero( void )
	{
		m_quat[0] = m_quat[1] = m_quat[2] = m_quat[3] = 0.0;
	};

	inline void Identity( void )
	{
		m_quat[0] = 0.0;
		m_quat[1] = 0.0;
		m_quat[2] = 0.0;
		m_quat[3] = 1.0;
	};

	inline void RotationAxis( Vector& p_axis, dsreal p_angle )
	{
		p_axis.Normalize();

		dsreal sin_a = sin( p_angle / 2.0 );

		m_quat[0] = p_axis[0] * sin_a;
		m_quat[1] = p_axis[1] * sin_a;
		m_quat[2] = p_axis[2] * sin_a;
		m_quat[3] = cos( p_angle / 2.0 );
	};

	inline void RotationMatFrom( Matrix& p_mat ) const
	{
		dsreal xx      = m_quat[0] * m_quat[0];
		dsreal xy      = m_quat[0] * m_quat[1];
		dsreal xz      = m_quat[0] * m_quat[2];
		dsreal xw      = m_quat[0] * m_quat[3];
		dsreal yy      = m_quat[1] * m_quat[1];
		dsreal yz      = m_quat[1] * m_quat[2];
		dsreal yw      = m_quat[1] * m_quat[3];
		dsreal zz      = m_quat[2] * m_quat[2];
		dsreal zw      = m_quat[2] * m_quat[3];

		p_mat( 0, 0 ) = 1 - 2 * ( yy + zz );
		p_mat( 1, 0 ) =     2 * ( xy - zw );
		p_mat( 2, 0 ) =     2 * ( xz + yw );
		p_mat( 0, 1 ) =     2 * ( xy + zw );
		p_mat( 1, 1 ) = 1 - 2 * ( xx + zz );
		p_mat( 2, 1 ) =     2 * ( yz - xw );
		p_mat( 0, 2 ) =     2 * ( xz - yw );
		p_mat( 1, 2 ) =     2 * ( yz + xw );
		p_mat( 2, 2 ) = 1 - 2 * ( xx + yy );
		p_mat( 3, 0 ) = p_mat( 3, 1 ) = p_mat( 3, 2 ) = p_mat( 0, 3 ) = p_mat( 1, 3 ) = p_mat( 2, 3 ) = 0;
		p_mat( 3, 3 ) = 1;
	};

	inline void Normalize(void)
	{
		dsreal a = m_quat[0];
		dsreal b = m_quat[1];
		dsreal c = m_quat[2];
		dsreal d = m_quat[3];

		dsreal n = sqrt(a * a + b * b + c * c + d * d);

		m_quat[0] = a / n;
		m_quat[1] = b / n;
		m_quat[2] = c / n;
		m_quat[3] = d / n;
	}

	static Quaternion Lerp(const Quaternion& p_q1, const Quaternion& p_q2, dsreal p_blend);
};
}
}

DrawSpace::Utils::Quaternion operator* ( DrawSpace::Utils::Quaternion p_qA, DrawSpace::Utils::Quaternion p_qB );

#endif
