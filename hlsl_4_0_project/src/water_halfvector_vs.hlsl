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
   float3 Position      : POSITION;   
};

struct VS_OUTPUT 
{
   float4 Position      : SV_POSITION;
   float4 Half0         : TEXCOORD4;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{    
    VS_OUTPUT Output;
    float4 pos;
    float4 pos2;
    
    pos.xyz = Input.Position;    
    pos.w = 1.0;
    Output.Position = mul(pos, mat[matWorldViewProjection]);

    // compute half vector
    float4x4 mat_World = mat[matWorld];
    float4x4 mat_Cam = mat[matCam];
    float4 Light0_Dir = vec[25];

    float4 Pos2 = mul(pos, mat_World);

    float4 CamPos;
    CamPos[0] = mat_Cam[3][0];
    CamPos[1] = mat_Cam[3][1];
    CamPos[2] = mat_Cam[3][2];
    CamPos[3] = 1.0;

    float3 nView = normalize(CamPos.xyz - Pos2.xyz);
    float3 nLight = normalize(-Light0_Dir.xyz);

    float4 H;
    H.xyz = nLight + nView;
    H.w = 0.0;

    Output.Half0 = H;
      
    return( Output );   
}
