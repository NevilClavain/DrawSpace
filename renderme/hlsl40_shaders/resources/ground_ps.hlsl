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

cbuffer constargs : register(b0)
{
    float4 vec[512];
    Matrix mat[512];
};


Texture2D txDiffuse         : register(t0);
SamplerState sam            : register(s0);

struct PS_INTPUT 
{
    float4 Position     : SV_POSITION;
	float2 TexCoord0    : TEXCOORD0;
    float4 TexCoord1    : TEXCOORD1;
    float  Fog          : FOG;
};

float ComputeExp2Fog(float depth, float density)
{
    float4 d = abs(depth);
    return 1 / exp2(d * density);
}

float4 fractal_texture(Texture2D tex, SamplerState sam, float2 uv, float depth)
{
    float LOD = log(depth);
    float LOD_floor = floor(LOD);
    float LOD_fract = LOD - LOD_floor;
    
    float2 uv1 = uv / exp(LOD_floor - 1.0);
    float2 uv2 = uv / exp(LOD_floor + 0.0);
    float2 uv3 = uv / exp(LOD_floor + 1.0);
    
    float4 tex0 = tex.Sample(sam, uv1);
    float4 tex1 = tex.Sample(sam, uv2);
    float4 tex2 = tex.Sample(sam, uv3);
    
    return (tex1 + lerp(tex0, tex2, LOD_fract)) * 0.5;
}

float4 ps_main(PS_INTPUT input) : SV_Target
{          
    float4 vw_pos = input.TexCoord1;
    
    float pixel_depth = 0.04 * abs(vw_pos.z);
    float4 tex_color = fractal_texture(txDiffuse, sam, input.TexCoord0, pixel_depth);
    
    float4 fog_color = vec[0];
    float4 fog_density = vec[1].x;
        
    float4 final_color = saturate(lerp(fog_color, tex_color, ComputeExp2Fog(vw_pos.z, fog_density)));
           
    return final_color;   
}
