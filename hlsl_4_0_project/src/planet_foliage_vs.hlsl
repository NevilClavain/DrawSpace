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

#include "mat_input_constants.hlsl"

struct VS_INPUT
{
    float3 Position     : POSITION;
    float3 Normal       : NORMALE;
    float4 TexCoord0    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float2 TexCoord0    : TEXCOORD0;
    float4 Normale      : TEXCOORD1;

    float Fog           : FOG;
};

#define v_viewer_pos                28

#define v_atmo_scattering_flag_0    42
#define v_atmo_scattering_flag_1    43
#define v_atmo_scattering_flag_2    44
#define v_atmo_scattering_flag_3    45
#define v_atmo_scattering_flag_4    46
#define v_atmo_scattering_flag_5    47
#define v_atmo_scattering_flag_6    48

#define v_flags_lights              50
#define v_ambient_color             51

#define v_light0_dir_local          52
#define v_light0_dir                53
#define v_light0_color              54

#define v_light1_dir_local          55
#define v_light1_dir                56
#define v_light1_color              57

#define v_light2_dir_local          58
#define v_light2_dir                59
#define v_light2_color              60



#include "spherelod_commons.hlsl"
#include "atmospheric_scattering.hlsl"


VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;

    float4 pos;
    pos.xyz = Input.Position;
    pos.w = 1.0;

    pos.xyz = pos.xzy; // swap y and z 



    Output.Position = mul(pos, mat[matWorldViewProjection]);
    Output.TexCoord0 = Input.TexCoord0.xy;

    Output.Normale.xyz = normalize(Input.Normal.xzy); // swap y and z 
    Output.Normale.w = 1.0;

    //// fog ////

    float4 viewer_pos = vec[v_viewer_pos];
    float4 atmo_scattering_flag_0 = vec[v_atmo_scattering_flag_0];
    float4 atmo_scattering_flag_5 = vec[v_atmo_scattering_flag_5];

    float4 PositionWV = mul(pos, mat[matWorldView]);

    float alt = length(viewer_pos) - atmo_scattering_flag_0.y;

    float fog_factor = saturate(atmo_scattering_flag_5.y / alt);
    Output.Fog = saturate(ComputeExp2Fog(PositionWV, lerp(0.0, atmo_scattering_flag_5.z, fog_factor)));

    /////////////

    return (Output);
}
