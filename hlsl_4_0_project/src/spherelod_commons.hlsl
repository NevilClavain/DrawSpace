/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#define UNDERWATER_GROUND_AMPLITUDE_FACTOR 7.0

float3 ProjectVectorToCube( int p_orientation, float3 p_vector )
{
    float3 res;

	if( 0 == p_orientation ) // front
	{
		res.x = p_vector.x;
		res.y = p_vector.y;
		res.z = p_vector.z;   
	}
	else if( 1 == p_orientation ) // rear
	{
		res.x = -p_vector.x;
		res.y = p_vector.y;
		res.z = -p_vector.z;   
	}
	else if( 2 == p_orientation ) // left
	{
		res.x = -p_vector.z;
		res.y = p_vector.y;
		res.z = p_vector.x;   
	}
	else if( 3 == p_orientation ) // right
	{
		res.x = p_vector.z;
		res.y = p_vector.y;
		res.z = -p_vector.x;   
	}
	else if( 4 == p_orientation ) // top
	{
		res.x = p_vector.x;
		res.y = p_vector.z;
		res.z = -p_vector.y;   
	}
	else //if( 5 == p_orientation ) // bottom
	{
		res.x = p_vector.x;
		res.y = -p_vector.z;
		res.z = p_vector.y;
	}

    return res;
}

float3 CubeToSphere( float3 p_vector )
{
    float3 res;
	float xtemp = p_vector.x;
	float ytemp = p_vector.y;
	float ztemp = p_vector.z;

	res.x = xtemp * sqrt( 1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp / 3.0 );
	res.y = ytemp * sqrt( 1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp / 3.0 );
	res.z = ztemp * sqrt( 1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp / 3.0 );

    return res;
}

// a partir des textures coords sur range [0 - 1], genere un center_point comme sur un patch front de vertex shader
float3 compute_front_face_point_vector( float2 p_tex_coords )
{
	float3 texel_pos = 0.0;

	// x - u -> passage de [0, 1] � [-1, 1]
	texel_pos.x = ( 2.0 * p_tex_coords.x ) - 1.0;

	// y - v -> passage de [0, 1] � [+1,-1]
	texel_pos.y = -( ( 2.0 * p_tex_coords.y ) - 1.0 );

	texel_pos.z = 1.0;

    return texel_pos;
}

