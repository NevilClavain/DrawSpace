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

float4x4 matWorldViewProjection : register(c0);

float4x4 matWorld               : register(c8);
float4x4 matView                : register(c12);


struct VS_INPUT
{
    float3 Position     : POSITION;
    float3 Normal       : NORMAL0;
    float4 TexCoord0    : TEXCOORD0;
    float4 TexCoord1    : TEXCOORD1;
    float4 Color        : TEXCOORD2;
};

struct VS_OUTPUT
{
    float4 Position     : POSITION0;
    float2 TexCoord0    : TEXCOORD0;
    float2 TexCoord1    : TEXCOORD1;
    float4 Normale      : TEXCOORD2;
    float4 Color        : TEXCOORD3;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{

    float4x4 id = 0;
    id[0][0] = 1.0;
    id[1][1] = 1.0;
    id[2][2] = -1.0;
    id[3][3] = 1.0;
    float4x4 matView2 = mul(matView, id);

    VS_OUTPUT Output;
    float4 pos;
    pos.xyz = Input.Position;
    pos.w = 1.0;

    Output.Position = mul(pos, matWorldViewProjection);
    Output.TexCoord0 = Input.TexCoord0.xy;
    Output.TexCoord1 = Input.TexCoord1.xy;

    float4 normale;
    normale.xyz = Input.Normal;
    normale.w = 1.0;

    float4x4 mWorldView = mul(matWorld, matView2);

    mWorldView[3][0] = 0.0;
    mWorldView[3][1] = 0.0;
    mWorldView[3][2] = 0.0;

    Output.Normale = normalize(mul(normale, mWorldView));
    Output.Normale.w = 1.0;

    Output.Color = Input.Color;
            
    return (Output);
}



