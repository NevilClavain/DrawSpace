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

#include "mat_input_constants.hlsl"

struct VS_INPUT
{
    float3 Position     : POSITION;
    float4 TexCoord0    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float2 TexCoord0    : TEXCOORD0;
    float4 TexCoord1    : TEXCOORD1;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;
    float4 pos;
    pos.xyz = Input.Position;
    pos.w = 1.0;

    Output.Position = mul(pos, mat[matWorldViewProjection]);      
    Output.TexCoord0 = Input.TexCoord0.xy;
    
    float4 wvp = mul(pos, mat[matWorldView]);
    Output.TexCoord1 = wvp;
             
    return (Output);
}
