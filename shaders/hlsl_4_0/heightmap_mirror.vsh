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

#include "mat_input_constants.hlsl"

Texture2D txDiffuse : register(t0);
SamplerState sam : register(s0);

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 TexCoord0 : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord0 : TEXCOORD0;
};

float4 reflectedVertexPos(float4 p_vertex, float4 p_reflectorPos, float4 p_reflectorNormale, float4x4 p_matWorld, float4x4 p_matView, float4x4 p_matProj)
{
    float4 rvp;

    float4x4 id = 0;
    id[0][0] = 1.0;
    id[1][1] = 1.0;
    id[2][2] = -1.0;
    id[3][3] = 1.0;
    float4x4 matView2 = mul(p_matView, id);
    
    float4 worldPos = mul(p_vertex, p_matWorld);
    float4 worldPos2 = worldPos - p_reflectorPos;
    worldPos2.w = 1.0;
    
    float4 worldPosRefl;
    worldPosRefl.xyz = reflect(worldPos2.xyz, normalize(p_reflectorNormale.xyz));
    worldPosRefl.w = worldPos.w;

    float4 worldPos3 = worldPosRefl + p_reflectorPos;
    worldPos3.w = 1.0;

    float4 viewPos = mul(worldPos3, matView2);

    rvp = mul(viewPos, p_matProj);
    return rvp;
}

VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;
    float4 pos;
    
    pos.xyz = Input.Position;
    pos.w = 1.0;

    pos.y = -2.0;

    float color_height = txDiffuse.SampleLevel(sam, Input.TexCoord0.xy, 0).x;
    pos.y = pos.y + (10.0 * color_height);


    Output.Position = reflectedVertexPos(pos, vec[24], vec[25], mat[matWorld], mat[matView], mat[matProj]);
    Output.TexCoord0 = Input.TexCoord0.xy;
      
    return (Output);
}
