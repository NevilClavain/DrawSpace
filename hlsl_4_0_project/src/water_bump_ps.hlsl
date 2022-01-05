/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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


Texture2D Texture0              : register(t0);
SamplerState SamplerTexture0    : register(s0);


struct PS_INTPUT 
{
    float4 Position : SV_POSITION;
	float2 TexCoord0: TEXCOORD0;
};

#include "mat_input_constants.hlsl"

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
    float4 bump_bias = vec[0];
    float4 surface_normale = vec[1];

    float4 res_color = 0;

    float3 np;
    np = compute_water_bump_vector(512, Texture0, SamplerTexture0, input.TexCoord0.xy, bump_bias.x);
    float3 nbase_t = surface_normale;
    res_color.xyz = np.xzy - nbase_t;

    return res_color;
}
