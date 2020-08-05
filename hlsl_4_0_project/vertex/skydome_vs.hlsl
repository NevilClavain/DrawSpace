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

#include ".\..\common\mat_input_constants.hlsl"

#define v_viewer_pos                28

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 TexCoord0 : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 t0		: TEXCOORD0;
    float4 t1		: TEXCOORD1;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;
    float4 pos;
    pos.xyz = Input.Position;
    pos.w = 1.0;

    float4x4 mat_Cam = mat[matCam];

    float4 viewer_pos;    // view pos relatif au centre de la sphere...
    viewer_pos.x = mat_Cam[3][0];
    viewer_pos.y = mat_Cam[3][1];
    viewer_pos.z = mat_Cam[3][2];
    viewer_pos.w = 1.0;

    Output.Position = mul(pos, mat[matWorldViewProjection]);

    ////// atmo scattering : calcul vertex pos

    float4x4 matWorldRot = mat[matWorld];

    // clear translations matWorld
    matWorldRot[3][0] = 0.0;
    matWorldRot[3][1] = 0.0;
    matWorldRot[3][2] = 0.0;

    float4 vertex_pos = mul(pos, matWorldRot);

    Output.t0 = vertex_pos;
    Output.t1 = viewer_pos;
      
    return (Output);
}
