/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
#include "generic_rendering.hlsl"

struct VS_INPUT
{
    float3 Position     : POSITION;
    float3 Normal       : NORMALE;
    float3 Tangent      : TANGENT;
    float3 Binormale    : BINORMALE;
    float4 TexCoord0    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float2 TexCoord0    : TEXCOORD0;
    float4 Normale      : TEXCOORD1;
    float4 Tangent      : TEXCOORD2;
    float4 Binormale    : TEXCOORD3;
    float4 Half0        : TEXCOORD4;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
    float4x4 mat_WorldViewProj = mat[matWorldViewProjection];
    float4x4 mat_World = mat[matWorld];
    float4x4 mat_Cam = mat[matCam];

    float4 Lights_Enabled = vec[24];
    float4 Light0_Dir = vec[25];

    /////////////////////////////////////

    VS_OUTPUT Output;
    float4 pos;
    pos.xyz = Input.Position;
    pos.w = 1.0;

    Output.Position = mul(pos, mat_WorldViewProj);
    Output.TexCoord0 = Input.TexCoord0.xy;

    //Output.Normale = TransformedNormaleForLights(Input.Normal, mat_World);

    Output.Normale.xyz = Input.Normal;
    Output.Normale.w = 1.0;

    Output.Tangent.xyz = Input.Tangent;
    Output.Tangent.w = 1.0;

    Output.Binormale.xyz = Input.Binormale;
    Output.Binormale.w = 1.0;

    if (Lights_Enabled.x > 0.0)
    {
        Output.Half0 = HalfVectorForLights(pos, Light0_Dir, mat_Cam, mat_World);
    }
    else
    {
        Output.Half0 = 0.0;
    }
    return (Output);

}
