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

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normales : NORMALE;
    float4 TexCoord0 : TEXCOORD0;
    float4 Pos : TEXCOORD7;
    float4 Scale : TEXCOORD8;
    
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord0 : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;
    float4 vertexpos;

    float2 pos2D = vec[5].xy;

    vertexpos.xy = pos2D;
    vertexpos.z = 0.0;
    vertexpos.w = 1.0;

    if (Input.Normales.x == 1.0)    
    {
        vertexpos.x += -0.5 * Input.Scale.x;
        vertexpos.y += 0.5 * Input.Scale.y;
    }
    else if (Input.Normales.x == 2.0)
    {
        vertexpos.x += 0.5 * Input.Scale.x;
        vertexpos.y += 0.5 * Input.Scale.y;
    }
    else if (Input.Normales.x == 3.0)
    {
        vertexpos.x += 0.5 * Input.Scale.x;
        vertexpos.y += -0.5 * Input.Scale.y;
    }
    else
    {
        vertexpos.x += -0.5 * Input.Scale.x;
        vertexpos.y += -0.5 * Input.Scale.y;
    }
   
    vertexpos.xy += Input.Pos.xy;
    vertexpos.z = 0.0;
    vertexpos.w = 1.0;
    Output.Position = vertexpos;
    Output.TexCoord0 = Input.TexCoord0.xy;
    return (Output);
}
