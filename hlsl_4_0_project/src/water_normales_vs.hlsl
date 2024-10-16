/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
   float4 delta_cam     : TEXCOORD1;     // vertex pos in camera space (-> world x view)
};

VS_OUTPUT vs_main( VS_INPUT Input )
{    
    VS_OUTPUT Output;
    float4 pos;
    float4 pos2;
    
    pos.xyz = Input.Position;    
    pos.w = 1.0;
    Output.Position = mul(pos, mat[matWorldViewProjection]);

    
    float4x4 mat_Cam = mat[matCam];

    float3 viewer_pos;    // view pos relatif au centre de la sphere...
    viewer_pos.x = mat_Cam[3][0];
    viewer_pos.y = mat_Cam[3][1];
    viewer_pos.z = mat_Cam[3][2];
    

    pos.xyz = Input.Position;
    pos.w = 1.0;
    pos2 = mul(pos, mat[matWorld]);

    float3 delta_cam;
    delta_cam.xyz = viewer_pos.xyz - pos2.xyz;

    float4 surface_normale = vec[10];

    Output.delta_cam.x = 0;
    Output.delta_cam.y = 0;
    Output.delta_cam.z = dot(normalize(delta_cam), surface_normale.xyz);
      
    return( Output );   
}
