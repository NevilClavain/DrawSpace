/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
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
};

#include "commons.hlsl"

float4 ps_main(PS_INTPUT input) : SV_Target
{
    float4 vw_pos = input.TexCoord1;        
    float pixel_depth = 0.04 * abs(vw_pos.z);    
    
    float4 tex_color = txDiffuse.Sample(sam, input.TexCoord0);    
    float4 key_color = vec[24];
    
    if (tex_color.r == key_color.r && tex_color.g == key_color.g && tex_color.b == key_color.b)
    {
        clip(-1);
    }    
            
    float4 fog_color = vec[25];
    float4 fog_density = vec[26].x;
        
    float4 final_color = saturate(lerp(fog_color, tex_color, computeExp2Fog(vw_pos.z, fog_density)));
           
    return final_color;
}
