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

Texture2D WaveTexture               : register(t0);
SamplerState SamplerWave            : register(s0);

struct PS_INTPUT 
{
    float4 LODGlobalPatch_TexCoord	: TEXCOORD0;
    float4 UnitPatch_TexCoord		: TEXCOORD1;
    float4 GlobalPatch_TexCoord		: TEXCOORD2;
};

#include "mat_input_constants.hlsl"

#define v_flags             0
#define v_flags2            1
#define v_bump_flag         30

float3 compute_water_bump_vector(int p_texture_resol, Texture2D p_water_bump_texture, SamplerState p_ss, float2 p_tex_coords, float p_vector_bias)
{
    float3 avg = 0.0;

    float texel_size = 1.0 / p_texture_resol;

    float2 left_coords = p_tex_coords;
    left_coords.x -= texel_size;

    float2 right_coords = p_tex_coords;
    right_coords.x += texel_size;

    float2 up_coords = p_tex_coords;
    up_coords.y -= texel_size;

    float2 down_coords = p_tex_coords;
    down_coords.y += texel_size;

    float4 wb_left = p_water_bump_texture.Sample(p_ss, left_coords);
    float4 wb_right = p_water_bump_texture.Sample(p_ss, right_coords);
    float4 wb_up = p_water_bump_texture.Sample(p_ss, up_coords);
    float4 wb_down = p_water_bump_texture.Sample(p_ss, down_coords);
    float4 wb_center = p_water_bump_texture.Sample(p_ss, p_tex_coords);

    float3 vec_left;
    vec_left.x = -p_vector_bias;
    vec_left.y = 0.0;
    vec_left.z = (wb_left.x - wb_center.x);

    float3 vec_right;
    vec_right.x = p_vector_bias;
    vec_right.y = 0.0;
    vec_right.z = (wb_right.x - wb_center.x);

    float3 vec_up;
    vec_up.x = 0.0;
    vec_up.y = -p_vector_bias;
    vec_up.z = (wb_up.x - wb_center.x);

    float3 vec_down;
    vec_down.x = 0.0;
    vec_down.y = p_vector_bias;
    vec_down.z = (wb_down.x - wb_center.x);

    float3 vec1 = normalize(cross(vec_right, vec_down));
    float3 vec2 = normalize(cross(vec_down, vec_left));
    float3 vec3 = normalize(cross(vec_left, vec_up));
    float3 vec4 = normalize(cross(vec_up, vec_right));

    avg = normalize(0.25 * (vec1 + vec2 + vec3 + vec4));

    return avg;
}

float4 ps_main(PS_INTPUT input) : SV_Target
{       
    float4x4 mWorldView = mat[matWorldView_ps];

    float4 flags = vec[v_flags];
    float4 flags2 = vec[v_flags2];
    float4 bump_flag = vec[v_bump_flag];

    float bump_bias;

    float relative_alt = flags.x;

    float lod_level = flags2.z;

    float bump_bias_transition_high = 1.008;
    float bump_bias_transition_low = 1.004;
    float wave_scale = 0.35;
    
    if (relative_alt > 1.0)
    {
        if (relative_alt > bump_bias_transition_high)
        {
            bump_bias = 100.0;
        }
        else if (relative_alt <= bump_bias_transition_high && relative_alt > bump_bias_transition_low)
        {
            bump_bias = lerp(bump_flag.y, 100.0, (relative_alt - bump_bias_transition_low) / (bump_bias_transition_high - bump_bias_transition_low));
        }
        else
        {
            bump_bias = bump_flag.y;
        }
    }
    else
    {
        //if underwater
        bump_bias = 0.333;
    }

       
    float4 res_color = 0;
    float3 np;

    float2 uvcoords = input.GlobalPatch_TexCoord.xy * wave_scale;
  
    np = compute_water_bump_vector(bump_flag.x, WaveTexture, SamplerWave, uvcoords * pow(2.0, lod_level - 1), bump_bias);

    float4 np2;
    np2.x = np.x;
    np2.y = np.z;
    np2.z = np.y;
    np2.w = 1.0;
    
    float3 np_t = mul(np2, transpose(mWorldView));
    float3 nbase_t = mul(float4(0.0, 1.0, 0.0, 1.0), transpose(matWorldView));

    res_color.xyz = np_t - nbase_t;   
    res_color = np2;

    return res_color;    
}
