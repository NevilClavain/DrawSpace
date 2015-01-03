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

#include "quaternion.h"

using namespace DrawSpace::Utils;

Quaternion::Quaternion(void)
{
    Zero();
}

Quaternion::~Quaternion(void)
{
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
