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

struct VS_INPUT
{
    float3 Position     : POSITION;
    float3 Normal       : NORMALE;
    float4 TexCoord0    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float2 TexCoord0    : TEXCOORD0;
    float4 Normale      : TEXCOORD1;
    float4 Half0        : TEXCOORD2;
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

	// NORMAL : pour lumieres diffuses : NORMAL transformee (sans les translations) dans repere world
    float4x4 worldRot = mat[matWorld];
    worldRot[0][3] = 0.0;
    worldRot[1][3] = 0.0;
    worldRot[2][3] = 0.0;

    Output.Normale = mul(Input.Normal, worldRot);

    float4 CamPos;
    float4 Pos2;
    float3 nView;
    float3 nLight;

    CamPos[0] = mat_Cam[3][0];
    CamPos[1] = mat_Cam[3][1];
    CamPos[2] = mat_Cam[3][2];
    CamPos[3] = 1.0;

    Pos2 = mul(pos, mat_World);
    nView = normalize(CamPos.xyz - Pos2.xyz);

    if (Lights_Enabled.x > 0.0)
    {
        nLight = normalize(-Light0_Dir.xyz);
        Output.Half0.xyz = (nLight + nView);
        Output.Half0[3] = 0.0;
    }
    else
    {
        Output.Half0 = 0.0;
    } 
    return (Output);

}
