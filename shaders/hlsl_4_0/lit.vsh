/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

// Flags ->
//          
//          .x = 1.0 -> mirror mode activated
//          .y = fog intensity
//          .z = 1.0 -> rigged animation ON (take bones transformations)

struct VS_INPUT
{
    float3 Position     : POSITION;
    float3 Normal       : NORMALE;
    float3 Tangent      : TANGENT;
    float3 Binormale    : BINORMALE;
    float4 TexCoord0    : TEXCOORD0;
	float4 BonesId0      : TEXCOORD4;
	float4 Weights0      : TEXCOORD5;
	float4 BonesId1      : TEXCOORD6;
	float4 Weights1      : TEXCOORD7;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float2 TexCoord0    : TEXCOORD0;
    float4 Normale      : TEXCOORD1;
    float4 Tangent      : TEXCOORD2;
    float4 Binormale    : TEXCOORD3;
    float4 Half0        : TEXCOORD4;
    float  Fog          : FOG;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
    float4x4 mat_WorldViewProj = mat[matWorldViewProjection];
    float4x4 mat_WorldView = mat[matWorldView];
    float4x4 mat_World = mat[matWorld];
    float4x4 mat_Cam = mat[matCam];
    float4x4 mat_View = mat[matView];
    float4x4 mat_Proj = mat[matProj];


    float4 Lights_Enabled = vec[24];
    float4 Light0_Dir = vec[25];

    float4 Flags = vec[26];
    float4 reflectorPos = vec[27];
    float4 reflectorNormal = vec[28];

    /////////////////////////////////////

	float4x4 final_transform_from_bones = { { 1.0, 0.0, 0.0, 0.0 },
								{ 0.0, 1.0, 0.0, 0.0 },
								{ 0.0, 0.0, 1.0, 0.0 },
								{ 0.0, 0.0, 0.0, 1.0 }
							  };
	if (Flags.z > 0.0)
	{
		int4 boneId0;

		boneId0[0] = (int)Input.BonesId0.x;
		boneId0[1] = (int)Input.BonesId0.y;
		boneId0[2] = (int)Input.BonesId0.z;
		boneId0[3] = (int)Input.BonesId0.w;

		int4 boneId1;

		boneId1[0] = (int)Input.BonesId1.x;
		boneId1[1] = (int)Input.BonesId1.y;
		boneId1[2] = (int)Input.BonesId1.z;
		boneId1[3] = (int)Input.BonesId1.w;

		float4 weights0 = Input.Weights0;
		float4 weights1 = Input.Weights1;

		// reconstituer les 4 matrices

		float4x4 mat0 = GetTransformationMatrixForBone(boneId0[0], vec);
		float4x4 mat1 = GetTransformationMatrixForBone(boneId0[1], vec);
		float4x4 mat2 = GetTransformationMatrixForBone(boneId0[2], vec);
		float4x4 mat3 = GetTransformationMatrixForBone(boneId0[3], vec);

		float4x4 mat4 = GetTransformationMatrixForBone(boneId1[0], vec);
		float4x4 mat5 = GetTransformationMatrixForBone(boneId1[1], vec);
		float4x4 mat6 = GetTransformationMatrixForBone(boneId1[2], vec);
		float4x4 mat7 = GetTransformationMatrixForBone(boneId1[3], vec);


		final_transform_from_bones =
			mat0 * weights0[0] +
			mat1 * weights0[1] +
			mat2 * weights0[2] +
			mat3 * weights0[3] +
			mat4 * weights1[0] +
			mat5 * weights1[1] +
			mat6 * weights1[2] +
			mat7 * weights1[3];
	}

    VS_OUTPUT Output;

	float4 initial_pos;

	initial_pos.xyz = Input.Position;
	initial_pos.w = 1.0;

    float4 pos;
    pos = mul(initial_pos, final_transform_from_bones);

    if (Flags.x > 0.0)
    {
        Output.Position = reflectedVertexPos(pos, reflectorPos, reflectorNormal, mat_World, mat_View, mat_Proj);
    }
    else
    {
        Output.Position = mul(pos, mat_WorldViewProj);
    }

    Output.Normale.xyz = Input.Normal;
    Output.Normale.w = 1.0;

    Output.Tangent.xyz = Input.Tangent;
    Output.Tangent.w = 1.0;

    Output.Binormale.xyz = Input.Binormale;
    Output.Binormale.w = 1.0;

    Output.TexCoord0 = Input.TexCoord0.xy;

    if (Lights_Enabled.x > 0.0)
    {
        Output.Half0 = HalfVectorForLights(pos, Light0_Dir, mat_Cam, mat_World);
    }
    else
    {
        Output.Half0 = 0.0;
    }

    float4 PositionWV = mul(pos, mat_WorldView);
    Output.Fog = clamp(0.0, 1.0, ComputeExp2Fog(PositionWV, Flags.y));

    return (Output);
}
