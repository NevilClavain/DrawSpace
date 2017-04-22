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

float4 base_uv_global : register(c27);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2


float4 landscape_control: register(c30);
	// .x -> plains amplitude
	// .y -> mountains amplitude
	// .z -> terrain offset
    // .w -> mountains offset

float4 seeds: register(c31);
	// .x -> plains seed 1
	// .y -> plains seed 2
	// .z -> mix seed 1
	// .w -> mix seed 2

sampler2D TextureRivers : register(s0);
sampler2D TextureCanyons : register(s1);

struct VS_INPUT 
{
    float4 Position : POSITION0;
    float4 TexCoord0 : TEXCOORD0;
};

struct VS_OUTPUT 
{
    float4 Position	: POSITION0;
    float4 alt		: TEXCOORD0;
};

#include "fbm.hlsl"
#include "multifbm_height.hlsl"
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


    float res = ComputeVertexHeight(v_position2, landscape_control.x, landscape_control.y, landscape_control.z, landscape_control.w, seeds.x, seeds.y, seeds.z, seeds.w);


    float4 global_uv = 0.0;
    global_uv.x = lerp(base_uv_global.x, base_uv_global.z, Input.TexCoord0.x);
    global_uv.y = lerp(base_uv_global.y, base_uv_global.w, Input.TexCoord0.y);
    /*
    res += ComputeCanyonsFromTexture(TextureCanyons, v_position2, global_uv, seeds.z, seeds.w);
    res *= ComputeRiversFromTexture(TextureRivers, v_position2, global_uv, seeds.z, seeds.w);
	*/
	Output.alt = 0.0;
	Output.alt.x = res;


	Output.Position = mul( Input.Position, matWorldViewProjection );
				  
	return( Output );   
}
