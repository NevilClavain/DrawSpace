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

float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);
float4x4 matWorld : register(c8);


float4   flag0:				register(c24);

	// .x -> patch orientation enum integer
	// .y -> patch sidelenght
	// .z -> planet ray
	// .w -> relative alt
	

float4   patch_translation:	register(c25);
	// .x, .y -> patch positionning

float4   base_uv: register(c26);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4   base_uv_global: register(c27);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2




struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0; 
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
};

#include "spherelod_commons.hlsl"

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	float4 v_position;

	// sidelenght scaling

	v_position = Input.Position * flag0.y / 2.0;
	v_position = v_position + patch_translation;
	v_position.z = 1.0;
	v_position.w = 1.0;
	
	float4 v_position2;	
	v_position2.w = 1.0;
	v_position2.xyz = CubeToSphere( ProjectVectorToCube( flag0.x, v_position.xyz ) );

	// final scaling
	float4 v_position3;	
	v_position3 = v_position2 * flag0.z;	
	v_position3.w = 1.0;

	float4 v_position4 = mul( v_position3, matWorldView );
	float vertex_distance = sqrt( v_position4.x * v_position4.x + v_position4.y * v_position4.y + v_position4.z * v_position4.z );

	float viewer_alt = flag0.w * flag0.z;
	float horizon_limit = sqrt( viewer_alt * viewer_alt - flag0.z * flag0.z );

	float v_alt = 0.0;

	if( vertex_distance < 2.0 * horizon_limit )
	{		
        v_alt = 0.0;
        
		// seuls les vertex "non skirt" prennent en compte l'altitude calculee du vertex;
		// les vertex "skirt" ont toujours une altitude de zero

		if( Input.TexCoord0.z == 0.0 )
		{
			v_position3 *= ( 1.0 + ( v_alt / flag0.z ) );
		}
        else
        {
            v_position3 *= (1.0 + (-100.0 / flag0.z));
        }
	}


	v_position3.w = 1.0;

	Output.Position = mul( v_position3, matWorldViewProjection );
	
	return( Output );   
}
