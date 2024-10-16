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
    float4 t0						: TEXCOORD0;
    float4 t1						: TEXCOORD1;
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

#include "spherelod_commons.hlsl"
#include "atmospheric_scattering.hlsl"


float4 compute_from_space(float4 p_t0, float4 p_t1, float4 p_ldir)
{
    float4 color;

    atmo_scattering_sampling_result sampling = skyfromspace_atmo_scattering_sampling(p_t0.xyz, p_t1.xyz, p_ldir.xyz);
    color.xyz = atmo_scattering_color_result(sampling, p_ldir.xyz);
    color.w = color.z;

    return color;
}

float4 compute_from_surface(float4 p_t0, float4 p_t1, float4 p_ldir)
{
    float4 viewer_pos = vec[v_viewer_pos];
    float4 atmo_scattering_flag_0 = vec[v_atmo_scattering_flag_0];
    float4 atmo_scattering_flag_5 = vec[v_atmo_scattering_flag_5];


    float viewer_alt = length(viewer_pos) - atmo_scattering_flag_0.y;
    float atmo_thickness = atmo_scattering_flag_0.x - atmo_scattering_flag_0.y;

    float4 color;

    atmo_scattering_sampling_result sampling = skyfromspace_atmo_scattering_sampling(p_t0.xyz, p_t1.xyz, p_ldir.xyz);
    float3 color_up = atmo_scattering_color_result(sampling, p_ldir.xyz);

    atmo_scattering_sampling_result sampling_down = skyfromatmo_atmo_scattering_sampling(p_t0.xyz, p_t1.xyz, p_ldir.xyz);
    float3 color_down = atmo_scattering_color_result(sampling_down, p_ldir.xyz);
    
    float alt_factor1 = (viewer_alt - atmo_thickness) / (atmo_scattering_flag_5.x - atmo_thickness);

    if( alt_factor1 > 1.0 )
    {
        color.xyz = color_up;
        color.w = color.z;
    }
    else if( alt_factor1 < 0.0 )
    {
        color.xyz = color_down;
       
        float alt_factor2 = clamp(viewer_alt / atmo_scattering_flag_5.x, 0.0, 1.0);
        float atmo_alpha_alt = 1.0 - alt_factor2;

        // diminuer progressivement l'influence du bleu dans la composante alpha
        float blue = lerp(color.z, 1.0, atmo_alpha_alt);

        // ne commencer a laisser apparaitre les etoiles que lorsque le prodsca est tres proche de 0.0
        
        float atmo_alpha_dir = lerp(color.z, 1.0, atan(clamp(dot(p_ldir, normalize(viewer_pos)), 0.0, 1.0) * 4000.0) / 1.57);

        float atmo_alpha = atmo_alpha_dir * atmo_alpha_alt * blue; // moduler alpha avec le bleu et l'altitude et le prod sca vecteur lumiere
   
        color.w = atmo_alpha; //lerp(atmo_alpha, color.z, alt_factor1);

    }
    else
    {
        color.xyz = lerp(color_down, color_up, alt_factor1);
        color.w = color.z;
    }

    return color;
}

float4 ps_main(PS_INTPUT input) : SV_Target
{

    float4 flags = vec[v_flags];
    float4 flags2 = vec[v_flags2];
    float4 viewer_pos = vec[v_viewer_pos];
    float4 flags_lights = vec[v_flags_lights];

    float4 ambient_color = vec[v_ambient_color];

    float4 light0_dir_local = vec[v_light0_dir_local];
    float4 light0_dir_global = vec[v_light0_dir];
    float4 light0_color = vec[v_light0_color];

    float4 light1_dir_local = vec[v_light1_dir_local];
    float4 light1_dir_global = vec[v_light1_dir];
    float4 light1_color = vec[v_light1_color];

    float4 light2_dir_local = vec[v_light2_dir_local];
    float4 light2_dir_global = vec[v_light2_dir];
    float4 light2_color = vec[v_light2_color];
    
    float4 atmo_scattering_flag_0 = vec[v_atmo_scattering_flag_0];
    float4 atmo_scattering_flag_1 = vec[v_atmo_scattering_flag_1];
    float4 atmo_scattering_flag_2 = vec[v_atmo_scattering_flag_2];
    float4 atmo_scattering_flag_3 = vec[v_atmo_scattering_flag_3];
    float4 atmo_scattering_flag_4 = vec[v_atmo_scattering_flag_4];
    float4 atmo_scattering_flag_5 = vec[v_atmo_scattering_flag_5];
    float4 atmo_scattering_flag_6 = vec[v_atmo_scattering_flag_6];

    /////////////////////////////////////////////////////////////////////////

    float viewer_alt = length(viewer_pos) - atmo_scattering_flag_0.y;
    float atmo_thickness = atmo_scattering_flag_0.x - atmo_scattering_flag_0.y;

    float4 color = 0.0;

    float4 l0;
    float4 l1;
    float4 l2;

    l0 = light0_dir_global;
    l1 = light1_dir_global;
    l2 = light2_dir_global;
 
    
    if (viewer_alt >= atmo_scattering_flag_5.x)
    {
        if (flags_lights.y)
        {
            color += compute_from_space(input.t0, input.t1, l0);
        }
        
        if (flags_lights.z)
        {
            color += compute_from_space(input.t0, input.t1, l1);
        }

        if (flags_lights.w)
        {
            color += compute_from_space(input.t0, input.t1, l2);
        }
    }
    else
    {
        if (flags_lights.y)
        {
            color += compute_from_surface(input.t0, input.t1, l0);
        }

        if (flags_lights.z)
        {
            color += compute_from_surface(input.t0, input.t1, l1);
        }

        if (flags_lights.w)
        {
            color += compute_from_surface(input.t0, input.t1, l2);
        }
    }


    float4 final_color;
    final_color.w = color.w;

    // added S. O'Neil HDR correction
    final_color.xyz = 1.0 - exp(-0.91 * color.xyz);
    

    return final_color;

}
