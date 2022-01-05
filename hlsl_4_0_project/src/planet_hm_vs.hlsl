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

#include "mat_input_constants.hlsl"

#define v_flag0                     24
#define v_patch_translation         25
#define v_base_uv                   26
#define v_base_uv_global            27


#define v_landscape_control         40
#define v_seeds                     41

struct VS_INPUT
{
    float3 Position     : POSITION;
    float4 TexCoord0    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float4 alt          : TEXCOORD0;
};

#include "fbm.hlsl"
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"

VS_OUTPUT vs_main(VS_INPUT Input)
{    
    VS_OUTPUT Output;
   
    float4 flag0 = vec[v_flag0];
    float4 patch_translation = vec[v_patch_translation];
    float4 base_uv = vec[v_base_uv];
    float4 base_uv_global = vec[v_base_uv_global];
    float4 landscape_control = vec[v_landscape_control];
    float4 seeds = vec[v_seeds];

    float4 v_position;
   		
	// sidelenght scaling

    v_position.xyz = Input.Position * flag0.y / 2.0;
    v_position.xy = v_position.xy + patch_translation.xy;
    v_position.z = 1.0;
    v_position.w = 1.0;
	
    float4 v_position2;
    v_position2.w = 1.0;
    v_position2.xyz = CubeToSphere(ProjectVectorToCube(flag0.x, v_position.xyz));
	
    float res = ComputeVertexHeight(v_position2, landscape_control.x, landscape_control.y, landscape_control.z, landscape_control.w, seeds.x, seeds.y, seeds.z, seeds.w);

    if (res < 0.0)
    {
        //if ground point is under sea level, amplify ground variations
        res *= UNDERWATER_GROUND_AMPLITUDE_FACTOR;
    }


    float4 global_uv = 0.0;
    global_uv.x = lerp(base_uv_global.x, base_uv_global.z, Input.TexCoord0.x);
    global_uv.y = lerp(base_uv_global.y, base_uv_global.w, Input.TexCoord0.y);
    
    /*
    res += ComputeCanyonsFromTexture(TextureCanyons, v_position2, global_uv, seeds.z, seeds.w);
    res *= ComputeRiversFromTexture(TextureRivers, v_position2, global_uv, seeds.z, seeds.w);
    */

    Output.alt = 0.0;
    Output.alt.x = res;
    
    float4 pos;
    pos.xyz = Input.Position;
    pos.w = 1.0;
    Output.Position = mul(pos, mat[matWorldViewProjection]);
    				  
    return (Output);
}
