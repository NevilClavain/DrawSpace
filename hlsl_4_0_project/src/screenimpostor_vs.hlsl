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
    float4 globalscale = vec[24];

    VS_OUTPUT Output;

    float4 centerpos = 0;
    centerpos.w = 1;
    float4 vertexpos;

    vertexpos.z = 0.0;
    vertexpos.w = 1.0;

    if (Input.Normales.x == 1.0)    
    {
        vertexpos.x = -0.5;
        vertexpos.y = 0.5;
    }
    else if (Input.Normales.x == 2.0)
    {
        vertexpos.x = 0.5;
        vertexpos.y = 0.5;
    }
    else if (Input.Normales.x == 3.0)
    {
        vertexpos.x = 0.5;
        vertexpos.y = -0.5;
    }
    else
    {
        vertexpos.x = -0.5;
        vertexpos.y = -0.5;
    }

    
    float4x4 inv = 0;

    inv[0][0] = 1.0;
    inv[1][1] = 1.0;
    inv[2][2] = -1.0;
    inv[3][3] = 1.0;

    float4x4 final_view = mul(mat[matView], inv);

    float4x4 localpos = 0;
    localpos[0][0] = 1.0;
    localpos[1][1] = 1.0;
    localpos[2][2] = 1.0;
    localpos[3][3] = 1.0;
    localpos[3][0] = Input.Pos.x;
    localpos[3][1] = Input.Pos.y;
    localpos[3][2] = Input.Pos.z;


	
    float4x4 world_view = mul(mul(localpos, mat[matWorld]), final_view);




    float4 vertexpos2 = mul(centerpos, world_view);
    vertexpos2.x += vertexpos.x * Input.Scale.x * globalscale.x;
    vertexpos2.y += vertexpos.y * Input.Scale.y * globalscale.y;
    vertexpos2.z += vertexpos.z;
    Output.Position = mul(vertexpos2, mat[matProj]);
    

    //Output.Position = mul(vertexpos, mat[matWorldViewProjection]);
    Output.TexCoord0 = Input.TexCoord0.xy;

    return (Output);
}
