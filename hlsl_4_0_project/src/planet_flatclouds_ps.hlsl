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

Texture2D       Clouds_Texture            : register(t0);
SamplerState    Clouds_Texture_Sampler    : register(s0);


struct PS_INTPUT
{
    float4 Position                 : SV_POSITION;

    float4 TexCoord0                : TEXCOORD0;
    float4 GlobalPatch_TexCoord     : TEXCOORD2;
};


#define v_flags                     0
#define v_flags2                    1
#define v_viewer_pos                2

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

#define v_clouds_texture_infos      29

#include "spherelod_commons.hlsl"


float3 CtoS(float3 p_v)
{
    float3 res;

    if (p_v.y > 0.0 && p_v.x == 0.0 && p_v.z == 0.0)
    {
        res.x = p_v.y;
        res.z = 3.1415927 / 2.0;
        res.y = 0.0;
        return res;
    }
    else if (p_v.y < 0.0 && p_v.x == 0.0 && p_v.z == 0.0)
    {
        res.x = -p_v.y;
        res.z = -3.1415927 / 2.0;
        res.y = 0.0;
        return res;
    }
    else
    {

        float normev = length(p_v);

        float ph, th;
        ph = asin(p_v.y / normev);

        th = atan2(p_v.x, p_v.z);
        res.y = th;
        res.z = ph;
        res.x = normev;

        return res;

    }
}


float4 ps_main(PS_INTPUT input) : SV_Target
{

    float4 flags = vec[v_flags];
    float4 flags_lights = vec[v_flags_lights];

    float4 ambient_color = vec[v_ambient_color];

    float4 light0_dir_local = vec[v_light0_dir_local];
    float4 light0_dir = vec[v_light0_dir];
    float4 light0_color = vec[v_light0_color];

    float4 light1_dir_local = vec[v_light1_dir_local];
    float4 light1_dir = vec[v_light1_dir];
    float4 light1_color = vec[v_light1_color];

    float4 light2_dir_local = vec[v_light2_dir_local];
    float4 light2_dir = vec[v_light2_dir];
    float4 light2_color = vec[v_light2_color];

    float4 clouds_texture_infos = vec[v_clouds_texture_infos];

    float pi = 3.1415927;
    float3 spherical = CtoS(input.TexCoord0.xyz);

    float4 final_color;
    float4 lit_color = 0.0;

    // repasser theta du range [-pi, pi] a [0, 2pi];
    spherical.y += pi;

    //ajout offset theta;
    //spherical.y += (clouds_texture_infos.z / 180.0) * pi;

    if (spherical.y >= 2 * pi)
    {
        spherical.y -= 2 * pi;
    }
    else if (spherical.y < 0.0)
    {
        spherical.y = (2 * pi) + spherical.y;
    }

    float2 uv;
    uv.x = (spherical.y) / (2.0 * pi);

    uv.y = 1.0 - ((spherical.z + (pi / 2.0)) / pi);

    final_color.w = Clouds_Texture.Sample(Clouds_Texture_Sampler, uv).xyz;
    
    float3 texel_pos = compute_front_face_point_vector(input.GlobalPatch_TexCoord.xy);
    texel_pos = normalize(texel_pos);

    float3 texel_pos2;
    texel_pos2 = CubeToSphere(ProjectVectorToCube(flags.w, texel_pos));

    float lights_dot_offset = 0.2;

    /*
    if (flags_lights.x > 0.0)
    {
        lit_color += ambient_color;
    }
    */

    if (flags_lights.y > 0.0)
    {
        lit_color += clamp(dot(texel_pos2, light0_dir_local.xyz) + lights_dot_offset, 0.0, 1.0) * light0_color;
    }

    if (flags_lights.z > 0.0)
    {
        lit_color += clamp(dot(texel_pos2, light1_dir_local.xyz) + lights_dot_offset, 0.0, 1.0) * light1_color;
    }

    if (flags_lights.w > 0.0)
    {
        lit_color += clamp(dot(texel_pos2, light2_dir_local.xyz) + lights_dot_offset, 0.0, 1.0) * light2_color;
    }

    final_color.xyz = lit_color.xyz;

    return final_color;
}
