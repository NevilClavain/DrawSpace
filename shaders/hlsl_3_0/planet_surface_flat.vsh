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
    // .z -> 1.0 = landplace patch, 0.0 otherwise

float4   base_uv: register(c26);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4   base_uv_global: register(c27);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4 viewer_pos : register(c28); // pos camera par rapport au centre sphere



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


float4 atmo_scattering_flag_0 : register(c32);
float4 atmo_scattering_flag_1 : register(c33);
float4 atmo_scattering_flag_2 : register(c34);
float4 atmo_scattering_flag_3 : register(c35);
float4 atmo_scattering_flag_4 : register(c36);
float4 atmo_scattering_flag_5 : register(c37);
float4 atmo_scattering_flag_6 : register(c38);


float4 flags_lights           : register(c40);
	// .x -> ambient light
	// .y -> color0 light
	// .z -> color1 light
	// .w -> color2 light

float4 ambient_color          : register(c41);

float4 light0_dir_local       : register(c42);
float4 light0_dir             : register(c43);
float4 light0_color           : register(c44);

float4 light1_dir_local       : register(c45);
float4 light1_dir             : register(c46);
float4 light1_color           : register(c47);

float4 light2_dir_local       : register(c48);
float4 light2_dir             : register(c49);
float4 light2_color           : register(c50);


sampler2D TextureRivers : register(s0);
sampler2D TextureCanyons : register(s1);


struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0; 
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float4 LODGlobalPatch_TexCoord	: TEXCOORD0;
   float4 UnitPatch_TexCoord		: TEXCOORD1;
   float4 GlobalPatch_TexCoord		: TEXCOORD2;

   float4 c0 : COLOR0;
   float4 c1 : COLOR1;

   float4 c0_1 : COLOR2;
   float4 c1_1 : COLOR3;

   float4 c0_2 : COLOR4;
   float4 c1_2 : COLOR5;

   float Fog : FOG;
};

#include "fbm.hlsl"
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"
#include "atmospheric_scattering.hlsl"

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	float4 v_position;
    float4 v_position3;

    if (patch_translation.z > 0.0 )
    {
        v_position3 = Input.Position;
    }
    else
    {
	    // sidelenght scaling

        v_position = Input.Position * flag0.y / 2.0;
        v_position = v_position + patch_translation;
        v_position.z = 1.0;
        v_position.w = 1.0;
	
        float4 v_position2;
        v_position2.w = 1.0;
        v_position2.xyz = CubeToSphere(ProjectVectorToCube(flag0.x, v_position.xyz));

	    // final scaling

        v_position3 = v_position2 * flag0.z;
        v_position3.w = 1.0;

    }

	Output.Position = mul( v_position3, matWorldViewProjection );
	
	Output.LODGlobalPatch_TexCoord = 0.0;
	Output.LODGlobalPatch_TexCoord.x = lerp( base_uv.x, base_uv.z, Input.TexCoord0.x );
	Output.LODGlobalPatch_TexCoord.y = lerp( base_uv.y, base_uv.w, Input.TexCoord0.y );

    Output.LODGlobalPatch_TexCoord.z = 0.0;
    Output.LODGlobalPatch_TexCoord.w = 0.0;

	// conserver aussi les coords textures originales du patch
	Output.UnitPatch_TexCoord = 0.0;
	Output.UnitPatch_TexCoord.x = Input.TexCoord0.x;
	Output.UnitPatch_TexCoord.y = Input.TexCoord0.y;
    Output.UnitPatch_TexCoord.z = patch_translation.z;

	
	Output.GlobalPatch_TexCoord = 0.0;
	Output.GlobalPatch_TexCoord.x = lerp( base_uv_global.x, base_uv_global.z, Input.TexCoord0.x );
	Output.GlobalPatch_TexCoord.y = lerp( base_uv_global.y, base_uv_global.w, Input.TexCoord0.y );


    Output.c0 = 0.0;
    Output.c1 = 0.0;
    Output.c0_1 = 0.0;
    Output.c1_1 = 0.0;
    Output.c0_2 = 0.0;
    Output.c1_2 = 0.0;
    
    Output.Fog = 0.0;

	return( Output );   
}
