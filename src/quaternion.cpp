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

#include "quaternion.h"

using namespace DrawSpace::Utils;

Quaternion::Quaternion(void)
{
    Zero();
}

Quaternion::Quaternion(dsreal p_x, dsreal p_y, dsreal p_z, dsreal p_w)
{
	m_quat[0] = p_x;
	m_quat[1] = p_y;
	m_quat[2] = p_z;
	m_quat[3] = p_w;
}

Quaternion::~Quaternion(void)
{
}

Quaternion Quaternion::Lerp(const Quaternion& p_q1, const Quaternion& p_q2, dsreal p_blend)
{
	Quaternion q1 = p_q1;
	Quaternion q2 = p_q2;
	Quaternion result;
	
	q1.Normalize();
	q2.Normalize();

	dsreal dot_product = p_q1[0] * p_q2[0] + p_q1[1] * p_q2[1] + p_q1[2] * p_q2[2] + p_q1[3] * p_q2[3];
	dsreal one_minus_blend = 1.0 - p_blend;

	dsreal sign = dot_product < 0.0 ? -1.0 : 1.0;

	result[0] = p_q1[0] * one_minus_blend + p_blend * sign * p_q2[0];
	result[1] = p_q1[1] * one_minus_blend + p_blend * sign * p_q2[1];
	result[2] = p_q1[2] * one_minus_blend + p_blend * sign * p_q2[2];
	result[3] = p_q1[3] * one_minus_blend + p_blend * sign * p_q2[3];

	result.Normalize();
	return result;
}

Quaternion operator* ( Quaternion p_qA, Quaternion p_qB )
{
	Quaternion res;

	res[0] = p_qB[3] * p_qA[0] + p_qB[0] * p_qA[3] + p_qB[1] * p_qA[2] - p_qB[2] * p_qA[1];
	res[1] = p_qB[3] * p_qA[1] - p_qB[0] * p_qA[2] + p_qB[1] * p_qA[3] + p_qB[2] * p_qA[0];
	res[2] = p_qB[3] * p_qA[2] + p_qB[0] * p_qA[1] - p_qB[1] * p_qA[0] + p_qB[2] * p_qA[3];
	res[3] = p_qB[3] * p_qA[3] - p_qB[0] * p_qA[0] - p_qB[1] * p_qA[1] - p_qB[2] * p_qA[2];

	return res;
}
