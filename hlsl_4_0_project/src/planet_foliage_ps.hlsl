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

cbuffer legacyargs : register(b0)
{
    float4 vec[512];
    Matrix mat[512];
};

#include "spherelod_commons.hlsl"
#include "mat_input_constants.hlsl"
#include "generic_rendering.hlsl"

#define v_flags_lights              7

#define v_ambient_color             8
#define v_light0_dir_local          9
#define v_light0_dir                10
#define v_light0_color              11

#define v_light1_dir_local          12
#define v_light1_dir                13
#define v_light1_color              14

#define v_light2_dir_local          15
#define v_light2_dir                16
#define v_light2_color              17

#define v_atmo_scattering_flag_0    18
#define v_atmo_scattering_flag_1    19
#define v_atmo_scattering_flag_2    20
#define v_atmo_scattering_flag_3    21
#define v_atmo_scattering_flag_4    22
#define v_atmo_scattering_flag_5    23
#define v_atmo_scattering_flag_6    24

Texture2D txDiffuse         : register(t0);
SamplerState sam            : register(s0);

struct PS_INTPUT 
{
    float4 Position  : SV_POSITION;
	float2 TexCoord0 : TEXCOORD0;
    float4 Normale   : TEXCOORD1;

    float Fog : FOG;
};


float4 ps_main(PS_INTPUT input) : SV_Target
{
    float4 color = txDiffuse.Sample(sam, input.TexCoord0);
    if (color.r < 0.1 && color.g < 0.1 && color.b < 0.1)
    {
        clip(-1.0);
    }

    float4 atmo_scattering_flag_6 = vec[v_atmo_scattering_flag_6];
    float4 fog_color = atmo_scattering_flag_6;

    float4 final_color = 1.0;
    float4 lit_color = 0.0;
    

    float4 pixel_color = color;


    //// lit computing
    float4x4 mat_World = mat[matWorld_ps];
    float4 world_normale = TransformedNormaleForLights(input.Normale, mat_World);

    float4 flags_lights = vec[v_flags_lights];

    
    float4 light0_dir = vec[v_light0_dir];
    float4 light0_color = vec[v_light0_color];
    

    if (flags_lights.y > 0.0)
    {
        lit_color += clamp(dot(world_normale.xyz, light0_dir.xyz), 0.0, 1.0) * light0_color;
    }



    pixel_color = saturate(lerp(fog_color, pixel_color, input.Fog));    
    final_color.xyz = pixel_color.xyz * lit_color.xyz;



    final_color.w = SPHERELOD_FOLIAGE_ID;
    return final_color;
}
