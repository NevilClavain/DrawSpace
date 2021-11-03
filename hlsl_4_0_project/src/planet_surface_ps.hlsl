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

Texture2D       Pixels_HTMap_Texture            : register(t0);
SamplerState    Pixels_HTMap_Texture_Sampler    : register(s0);

Texture2D       Splatting_HTMap_Texture         : register(t1);
SamplerState    Splatting_HTMap_Texture_Sampler : register(s1);

Texture2D       HT_Texture                      : register(t7);
SamplerState    HT_Texture_sampler              : register(s7);

struct PS_INTPUT
{
    float4 Position                 : SV_POSITION;
    float4 LODGlobalPatch_TexCoord  : TEXCOORD0;
    float4 UnitPatch_TexCoord       : TEXCOORD1;
    float4 GlobalPatch_TexCoord     : TEXCOORD2;
    
    float4 c0 : COLOR0;
    float4 c1 : COLOR1;

    float4 c0_1 : COLOR2;
    float4 c1_1 : COLOR3;

    float4 c0_2 : COLOR4;
    float4 c1_2 : COLOR5;

    float Fog : FOG;  
};

#include "spherelod_commons.hlsl"
#include "landscapes.hlsl"

#define v_flags                     0
#define v_flags2                    1
#define v_viewer_pos                2
#define v_flags6                    6
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

#define m_matWorldRots              25

#define v_terrain_bump_flag         31

float4 ps_main(PS_INTPUT input) : SV_Target
{

    float4 flags = vec[v_flags];
    float4 flags2 = vec[v_flags2];
    float4 viewer_pos = vec[v_viewer_pos];
    float4 flags6 = vec[v_flags6];
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

    float4x4 matWorldRots = mat[m_matWorldRots];
    float4 terrain_bump_flag = vec[v_terrain_bump_flag];

    /////////////////////////////////////////////////////////////////////////

    float4 final_color = 1.0;
    float4 lit_color = 0.0;
    float4 pixel_color = 0.0;

    float4 temp_humidity = HT_Texture.Sample(HT_Texture_sampler, input.LODGlobalPatch_TexCoord.xy);

    float3 water_color = { 0.17, 0.36, 0.48 };

    bool water_mask_mode = false;

    float lim_sup = flags6.y;
    float lim_inf = flags6.z;

    bool sea = false;

    float pixel_distance = input.LODGlobalPatch_TexCoord.w;

    if (flags.x <= flags6.w)
    {
        water_mask_mode = true;
    }

    if (flags.x > lim_inf)
    {
        // si altitude relative > 1.5

        // regarder texture donnees temperature/humidite pour determiner si rendu mer
        if (temp_humidity.z > 0.0)
        {
            sea = true;
        }
    }

    float relative_alt = flags.w;

    float3 avg = 0.0;

    if (!sea)
    {
        avg = compute_terrain_bump_vector(temp_humidity.w, flags2.x, HT_Texture, HT_Texture_sampler, input.LODGlobalPatch_TexCoord.xy, terrain_bump_flag.x);
    }

    float3 texel_pos = compute_front_face_point_vector(input.GlobalPatch_TexCoord.xy);


    if (!sea)
    {
        float k = clamp((1.5708 - atan(30.0 * (flags.x - 1.0))), 0.01, 0.35);
        
        texel_pos.x += k * avg.x;
        texel_pos.y += k * -avg.y; // inversion sur l'axe y, car pour le repere u,v des textures l'axe v (y) est vers le bas
        
        texel_pos = normalize(texel_pos);

    }

    int count_lights = 0;
    float4 spec0 = 0;
    float4 spec1 = 0;
    float4 spec2 = 0;

    float3 nviewerpos = normalize(viewer_pos.xyz);
    
    float4 texel_pos2;
    texel_pos2.xyz = CubeToSphere(ProjectVectorToCube(flags.w, texel_pos));
    texel_pos2.w = 1.0;

    float4 normale_world = mul(matWorldRots, texel_pos2);

    float ocean_spec_power = 120.0;

    if (flags_lights.x > 0.0)
    {
        lit_color += ambient_color;
    }

    if (flags_lights.y > 0.0)
    {
        lit_color += clamp(dot(texel_pos2.xyz, light0_dir_local.xyz), 0.0, 1.0) * light0_color;
        if (sea)
        {
            float3 hv = normalize(light0_dir.xyz + nviewerpos);
            spec0 = ocean_specular_from_space(specular_light(hv, normale_world.xyz, ocean_spec_power), flags.x, light0_dir.xyz, hv);
        }
        count_lights++;
    }

    if (flags_lights.z > 0.0)
    {
        lit_color += clamp(dot(texel_pos2.xyz, light1_dir_local.xyz), 0.0, 1.0) * light1_color;
        if (sea)
        {
            float3 hv = normalize(light1_dir.xyz + nviewerpos);
            spec1 = ocean_specular_from_space(specular_light(hv, normale_world.xyz, ocean_spec_power), flags.x, light1_dir.xyz, hv);
        }
        count_lights++;
    }

    if (flags_lights.w > 0.0)
    {
        lit_color += clamp(dot(texel_pos2.xyz, light2_dir_local.xyz), 0.0, 1.0) * light2_color;
        if (sea)
        {
            float3 hv = normalize(light2_dir.xyz + nviewerpos);
            spec2 = ocean_specular_from_space(specular_light(hv, normale_world.xyz, ocean_spec_power), flags.x, light2_dir.xyz, hv);
        }
        count_lights++;
    }


	////////////////////////////////////////////////////////////////////////////////////

    float2 ddx = { 0.0, 0.0 };
    float2 ddy = { 0.0, 0.0 };
	
    if (flags.x > 0.0 && flags.x <= lim_inf)
    {
        if (sea)
        {
            pixel_color.xyz = water_color;
        }
        else
        {
            pixel_color = splatting_color(input.UnitPatch_TexCoord, temp_humidity.x, temp_humidity.y, flags6.x, Splatting_HTMap_Texture, Splatting_HTMap_Texture_Sampler);
        }

    }
    else if (flags.x > lim_inf && flags.x <= lim_sup)
    {
        float4 color_splat = splatting_color(input.UnitPatch_TexCoord, temp_humidity.x, temp_humidity.y, flags6.x, Splatting_HTMap_Texture, Splatting_HTMap_Texture_Sampler);
        float4 color_pixel = Pixels_HTMap_Texture.SampleGrad(Pixels_HTMap_Texture_Sampler, temp_humidity.xy, ddx, ddy); //tex2D(Pixels_HTMap_Texture, temp_humidity);

        if (sea)
        {
            pixel_color.xyz = water_color;
        }
        else
        {
            pixel_color = lerp(color_splat, color_pixel, (flags.x - lim_inf) / (lim_sup - lim_inf));
        }
    }
    else
    {
        if (sea)
        {
            pixel_color.xyz = water_color;
        }
        else
        {
            pixel_color = Pixels_HTMap_Texture.SampleGrad(Pixels_HTMap_Texture_Sampler, temp_humidity.xy, ddx, ddy); //tex2D(Pixels_HTMap_Texture, temp_humidity);
        }
    }

    float4 fog_color;

    if (relative_alt > 1.0)
    {
        fog_color = atmo_scattering_flag_6;
    }
    else
    {
        //underwater : fog color is water color
        fog_color.rgb = water_color;
        fog_color.a = 1.0;
    }

    ///////////////////// inclure le "fog de surface" dans la couleur pixel

    pixel_color = saturate(lerp(fog_color, pixel_color, input.Fog));

    /////////////////////

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
    
        final_color = c0_final + (lit_color * pixel_color * c1_final);
    }
    else
    {
        final_color = lit_color * pixel_color;
    }


    if (flags_lights.y > 0.0)
    {
        final_color += spec0;
    }

    if (flags_lights.z > 0.0)
    {
        final_color += spec1;
    }

    if (flags_lights.w > 0.0)
    {
        final_color += spec2;
    }

    final_color.xyz = saturate(final_color.xyz);


    if (count_lights > 0)
    {
        // pour eviter la saturation des couleurs dues a l'additivite
        final_color.xyz = final_color.xyz / count_lights;
    }

	/* ECH desactivation temporaire
    if (water_mask_mode)
    {
        if (sea)
        {
            //float4 real_color = c0_final + (lit_color * pixel_color * c1_final);
            final_color.w = 0.0;
        }
        else
        {
            final_color.w = 1.0;
        }
    }
    else
    {
        final_color.w = 1.0;
    }
	*/


    if (flags2.y > 0.0)
    {
        // for debug purpose (show current patch in purple)
        final_color.x = final_color.z = 1.0;
        final_color.y = 0.0;
    }

    final_color.w = 1.0;
    return final_color;
}
