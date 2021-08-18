/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                    
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

    float4 c0                       : COLOR0;
    float4 c1                       : COLOR1;

    float4 c0_1                     : COLOR2;
    float4 c1_1                     : COLOR3;

    float4 c0_2                     : COLOR4;
    float4 c1_2                     : COLOR5;
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
#include "landscapes.hlsl"


float3 CtoS(float3 p_v)
{
    float3 res;

    float normev = length(p_v);

    float ph, th;
    ph = asin(p_v.y / normev);

    th = atan2(p_v.x, p_v.z);
    res.y = th;
    res.z = ph;
    res.x = normev;

    return res;
}

float4 ps_main(PS_INTPUT input) : SV_Target
{

    float4 flags = vec[v_flags];
    float4 viewer_pos = vec[v_viewer_pos];

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

    float4 atmo_scattering_flag_0 = vec[v_atmo_scattering_flag_0];
    float4 atmo_scattering_flag_1 = vec[v_atmo_scattering_flag_1];
    float4 atmo_scattering_flag_2 = vec[v_atmo_scattering_flag_2];
    float4 atmo_scattering_flag_3 = vec[v_atmo_scattering_flag_3];
    float4 atmo_scattering_flag_4 = vec[v_atmo_scattering_flag_4];
    float4 atmo_scattering_flag_5 = vec[v_atmo_scattering_flag_5];
    float4 atmo_scattering_flag_6 = vec[v_atmo_scattering_flag_6];


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

  
    float3 avg = 0.0;
    avg = compute_clouds_bump_vector(8192, 4096, Clouds_Texture, Clouds_Texture_Sampler, uv, 0.333);


    float3 texel_pos = compute_front_face_point_vector(input.GlobalPatch_TexCoord.xy);

    texel_pos.x += avg.x;
    texel_pos.y += -avg.y; // inversion sur l'axe y, car pour le repere u,v des textures l'axe v (y) est vers le bas

    texel_pos = normalize(texel_pos);

    float3 texel_pos2;
    texel_pos2 = CubeToSphere(ProjectVectorToCube(flags.w, texel_pos));

    float lights_dot_offset = 0.2;

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

    //////////////////////////////////////////////////////////////////////////////////////

    float viewer_alt = length(viewer_pos) - atmo_scattering_flag_0.y;

    if (viewer_alt > 0.0 && atmo_scattering_flag_5.w > 0.0) // si calcul atmo autorise
    {
        float4 c0_final, c1_final;
        c0_final = c1_final = 0.0;

        if (viewer_alt >= atmo_scattering_flag_5.x)
        {
            if (flags_lights.y > 0.0)
            {
                c0_final += input.c0;
                c1_final += input.c1;
            }

            if (flags_lights.z > 0.0)
            {
                c0_final += input.c0_1;
                c1_final += input.c1_1;
            }

            if (flags_lights.w > 0.0)
            {
                c0_final += input.c0_2;
                c1_final += input.c1_2;
            }
        }
        else
        {

            // vue de l'interieur de l'atmo, on ne veut pas prendre en compte la composante c1 (provoque un rougoiement indesirable des
            // couleurs) -> remplacer progressivement par 1.0, en fct de l'altitude camera

            float factor_alt = 1.0 - clamp(viewer_alt / atmo_scattering_flag_5.x, 0.0, 1.0);

            if (flags_lights.y > 0.0)
            {
                c0_final += input.c0;
                c1_final += lerp(input.c1, 1.0, factor_alt);
            }

            if (flags_lights.z > 0.0)
            {
                c0_final += input.c0_1;
                c1_final += lerp(input.c1_1, 1.0, factor_alt);
            }

            if (flags_lights.w > 0.0)
            {
                c0_final += input.c0_2;
                c1_final += lerp(input.c1_2, 1.0, factor_alt);
            }
        }

        final_color.xyz = c0_final + (lit_color.xyz * c1_final);
    }
    else
    {
        final_color.xyz = lit_color.xyz;
    }
    
    //////////////////////////////////////////////////////////////////////////////////////

    float delta = 0.0035;

    if (abs(input.TexCoord0.x) < delta)
    {
        if (input.TexCoord0.z < delta)
        {
            // to fix the gray line artifact :-p
            uv.x = 1.0;
            final_color.w = Clouds_Texture.Sample(Clouds_Texture_Sampler, uv).xyz;
        }
        else
        {
            final_color.w = Clouds_Texture.Sample(Clouds_Texture_Sampler, uv).xyz;
        }
    }    
    else
    {
        final_color.w = Clouds_Texture.Sample(Clouds_Texture_Sampler, uv).xyz;
    }    
    return final_color;
}
