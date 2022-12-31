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
    float4 TexCoord3                : TEXCOORD3;

    float Fog : FOG;
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


float4 ps_main(PS_INTPUT input) : SV_Target
{
    float v_alt = input.TexCoord3.x;

    if (v_alt > 0.0)
    {
        clip(-1.0);
    }

    float4 flags = vec[v_flags];
    float4 flags_lights = vec[v_flags_lights];

    float4 ambient_color = vec[v_ambient_color];

    float4 lit_color = 0.0;

    float4 light0_dir_local = vec[v_light0_dir_local];
    float4 light0_dir = vec[v_light0_dir];
    float4 light0_color = vec[v_light0_color];

    float4 light1_dir_local = vec[v_light1_dir_local];
    float4 light1_dir = vec[v_light1_dir];
    float4 light1_color = vec[v_light1_color];

    float4 light2_dir_local = vec[v_light2_dir_local];
    float4 light2_dir = vec[v_light2_dir];
    float4 light2_color = vec[v_light2_color];

    

    float surface_dot_delta = abs(input.TexCoord3.y);




    float3 avg = 0.0;
    float3 texel_pos = compute_front_face_point_vector(input.GlobalPatch_TexCoord.xy);

    float4 texel_pos2;
    texel_pos2.xyz = CubeToSphere(ProjectVectorToCube(flags.w, texel_pos));
    texel_pos2.w = 1.0;

    int count_lights = 0;

    if (flags_lights.x > 0.0)
    {
        lit_color += ambient_color;
    }

    if (flags_lights.y > 0.0)
    {
        lit_color += clamp(dot(texel_pos2.xyz, light0_dir_local.xyz), 0.0, 1.0) * light0_color;
        count_lights++;
    }

    if (flags_lights.z > 0.0)
    {
        lit_color += clamp(dot(texel_pos2.xyz, light1_dir_local.xyz), 0.0, 1.0) * light1_color;
        count_lights++;
    }

    if (flags_lights.w > 0.0)
    {
        lit_color += clamp(dot(texel_pos2.xyz, light2_dir_local.xyz), 0.0, 1.0) * light2_color;
        count_lights++;
    }

    float avg_lit_color = 0.299 * lit_color.r + 0.587 * lit_color.r + 0.114 * lit_color.b;

    //////////////////////////////
    // output mask pixel :
    // .x -> avg_lit_color = ocean light intensity
    // .y -> surface_dot_delta = dot product between ocean surface normale and vector from this point to camera point
    // .z -> not used
    // .w -> fog density

    //////////////////////////////
    float4 water_color = { avg_lit_color, surface_dot_delta, 0.0, input.Fog };
   
    return water_color;
}
