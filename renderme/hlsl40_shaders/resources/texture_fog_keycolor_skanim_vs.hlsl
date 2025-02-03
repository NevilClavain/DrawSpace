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
#include "commons.hlsl"

struct VS_INPUT
{
    float3 Position     : POSITION;
    float4 TexCoord0    : TEXCOORD0;
    float4 TexCoord1    : TEXCOORD1;
    float4 TexCoord2    : TEXCOORD2;
    float4 TexCoord3    : TEXCOORD3;
    
       
    float4 BonesId0     : TEXCOORD4;
    float4 Weights0     : TEXCOORD5;
    float4 BonesId1     : TEXCOORD6;
    float4 Weights1     : TEXCOORD7;
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
    float4 initial_pos;
    
    float4 pos;
            
    int4 boneId0;
    boneId0[0] = (int) Input.BonesId0.x;
    boneId0[1] = (int) Input.BonesId0.y;
    boneId0[2] = (int) Input.BonesId0.z;
    boneId0[3] = (int) Input.BonesId0.w;

    int4 boneId1;
    boneId1[0] = (int) Input.BonesId1.x;
    boneId1[1] = (int) Input.BonesId1.y;
    boneId1[2] = (int) Input.BonesId1.z;
    boneId1[3] = (int) Input.BonesId1.w;
    
    float4 weights0 = Input.Weights0;
    float4 weights1 = Input.Weights1;
    
    // reconstituer les 4 matrices
    
    float4x4 mat0 = getTransformationMatrixForBone(boneId0[0], 24, vec);
    float4x4 mat1 = getTransformationMatrixForBone(boneId0[1], 24, vec);
    float4x4 mat2 = getTransformationMatrixForBone(boneId0[2], 24, vec);
    float4x4 mat3 = getTransformationMatrixForBone(boneId0[3], 24, vec);

    float4x4 mat4 = getTransformationMatrixForBone(boneId1[0], 24, vec);
    float4x4 mat5 = getTransformationMatrixForBone(boneId1[1], 24, vec);
    float4x4 mat6 = getTransformationMatrixForBone(boneId1[2], 24, vec);
    float4x4 mat7 = getTransformationMatrixForBone(boneId1[3], 24, vec);
    
    float4x4 final_transform_from_bones =
			mat0 * weights0[0] +
			mat1 * weights0[1] +
			mat2 * weights0[2] +
			mat3 * weights0[3] +
			mat4 * weights1[0] +
			mat5 * weights1[1] +
			mat6 * weights1[2] +
			mat7 * weights1[3];
    
    initial_pos.xyz = Input.Position;
    initial_pos.w = 1.0;
    
    pos = mul(initial_pos, final_transform_from_bones);
        
    Output.Position = mul(pos, mat[matWorldViewProjection]);
        
    Output.TexCoord0 = Input.TexCoord0.xy;
    
    float4 wvp = mul(pos, mat[matWorldView]);
    Output.TexCoord1 = wvp;
          
    return (Output);
}
