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


struct PS_INTPUT
{
    float4 Position                 : SV_POSITION;
    float4 LODGlobalPatch_TexCoord  : TEXCOORD0;
    float4 UnitPatch_TexCoord       : TEXCOORD1;
    float4 GlobalPatch_TexCoord     : TEXCOORD2;
    float3 Normale                  : TEXCOORD3;
    float3 Half0                    : TEXCOORD4;
    float3 Half1                    : TEXCOORD5;
    float3 Half2                    : TEXCOORD6;
};

#include "spherelod_commons.hlsl"

#define v_flags                     0

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

#define v_flag32                    32


float4 ps_main(PS_INTPUT input) : SV_Target
{
    float4 color = 0;
    float spec_power = vec[v_flag32].y;

    float4 flags_lights = vec[v_flags_lights];

    if (flags_lights.y > 0.0)
    {
        float dotNH = clamp(dot(input.Normale, input.Half0), 0.0, 1.0);
        color.xyz += pow(dotNH, spec_power);
    }

    if (flags_lights.z > 0.0)
    {
        float dotNH = clamp(dot(input.Normale, input.Half1), 0.0, 1.0);
        color.xyz += pow(dotNH, spec_power);
    }

    if (flags_lights.w > 0.0)
    {
        float dotNH = clamp(dot(input.Normale, input.Half2), 0.0, 1.0);
        color.xyz += pow(dotNH, spec_power);
    }
       
    color.w = 1;
    return color;
}
