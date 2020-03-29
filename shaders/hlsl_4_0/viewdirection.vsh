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

cbuffer legacyargs : register(b0)
{
    float4 vec[512];
    Matrix mat[512];
};

#include "mat_input_constants.hlsl"

struct VS_INPUT 
{
	float3 Position : POSITION;
	float4 TexCoord0 : TEXCOORD0;
};

struct VS_OUTPUT 
{
	float4 Position : SV_POSITION;
	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
    VS_OUTPUT Output;
    float4 pos;

	float4 camera_params = vec[24];

	float width = camera_params.x;
	float height = camera_params.y;
	float zn = camera_params.z;
    
    pos.xyz = Input.Position;    
    pos.w = 1.0;

    Output.Position = mul(pos, mat[matWorldViewProjection]);
	Output.TexCoord0 = Input.TexCoord0.xy;

	if (Input.TexCoord0.x == 0.0 && Input.TexCoord0.y == 0.0)
	{
		Output.TexCoord1.x = -width * 0.5;
		Output.TexCoord1.y = -height * 0.5;
	}
	else if (Input.TexCoord0.x == 1.0 && Input.TexCoord0.y == 0.0)
	{
		Output.TexCoord1.x = width;
		Output.TexCoord1.y = -height * 0.5;
	}
	else if (Input.TexCoord0.x == 1.0 && Input.TexCoord0.y == 1.0)
	{
		Output.TexCoord1.x = width * 0.5;
		Output.TexCoord1.y = height * 0.5;
	}
	else // (Input.TexCoord0.x == 0.0 && Input.TexCoord0.y == 1.0)
	{
		Output.TexCoord1.x = -width * 0.5;
		Output.TexCoord1.y = height * 0.5;
	}
      
    return( Output );   
}
