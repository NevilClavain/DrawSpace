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

float4x4 matWorldViewProjection     : register(c0);
float4x4 matWorldView               : register(c4);
float4x4 matWorld                   : register(c8);
float4x4 matView                    : register(c12);
float4x4 matCam                     : register(c16);
float4x4 matProj                    : register(c20);

float4 Lights_Enabled               : register(c24);
float4 Light0_Dir                   : register(c25);

float4 Flags                        : register(c26);
float4 reflectorPos                 : register(c27);
float4 reflectorNormal              : register(c28);


float4 bones_0[69]: register(c31);
float4 bones_1[132]: register(c124);


struct VS_INPUT 
{
   float4 Position      : POSITION0;
   float3 Normal        : NORMAL0;
   float3 Tangent       : TANGENT0;
   float3 Binormale     : BINORMAL0;
   float4 TexCoord0     : TEXCOORD0;
   float4 BonesId0      : TEXCOORD4;
   float4 Weights0      : TEXCOORD5;
   float4 BonesId1      : TEXCOORD6;
   float4 Weights1      : TEXCOORD7;

};

struct VS_OUTPUT 
{
    float4 Position     : POSITION0;
    float2 TexCoord0    : TEXCOORD0;
    float4 Normale      : TEXCOORD1;
    float4 Tangent      : TEXCOORD2;
    float4 Binormale    : TEXCOORD3;
    float4 Half0        : TEXCOORD4;

    float Fog           : FOG;
};

#include "generic_rendering.hlsl"

// Flags ->
//          
//          .x = 1.0 -> mirror mode activated
//          .y -> fog intensity

VS_OUTPUT vs_main( VS_INPUT Input )
{


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

		float4x4 mat0;
		float4x4 mat1;
		float4x4 mat2;
		float4x4 mat3;

		float4x4 mat4;
		float4x4 mat5;
		float4x4 mat6;
		float4x4 mat7;

		mat0 = GetTransformationMatrixForBone(boneId0[0], bones_0, bones_1);
		mat1 = GetTransformationMatrixForBone(boneId0[1], bones_0, bones_1);
		mat2 = GetTransformationMatrixForBone(boneId0[2], bones_0, bones_1);
		mat3 = GetTransformationMatrixForBone(boneId0[3], bones_0, bones_1);
		mat4 = GetTransformationMatrixForBone(boneId1[0], bones_0, bones_1);
		mat5 = GetTransformationMatrixForBone(boneId1[1], bones_0, bones_1);
		mat6 = GetTransformationMatrixForBone(boneId1[2], bones_0, bones_1);
		mat7 = GetTransformationMatrixForBone(boneId1[3], bones_0, bones_1);

		final_transform_from_bones = mat0 * weights0[0] + mat1 * weights0[1] + mat2 * weights0[2] + mat3 * weights0[3] + mat4 * weights1[0] + mat5 * weights1[1] + mat6 * weights1[2] + mat7 * weights1[3];
	}

	VS_OUTPUT Output;

	float4 initial_pos;
	float4 initial_n;
	float4 initial_t;
	float4 initial_b;

	initial_pos.xyz = Input.Position;
	initial_pos.w = 1.0;

	initial_n.xyz = Input.Normal;
	initial_n.w = 1.0;

	initial_t.xyz = Input.Tangent;
	initial_t.w = 1.0;

	initial_b.xyz = Input.Binormale;
	initial_b.w = 1.0;

	float4 pos;
	pos = mul(initial_pos, final_transform_from_bones);

    if (Flags.x == 1.0)
    {
        Output.Position = reflectedVertexPos(pos, reflectorPos, reflectorNormal, matWorld, matView, matProj);
    }
    else
    {
        Output.Position = mul(pos, matWorldViewProjection);
    }

	float4x4 final_transform_from_bones_no_translation = final_transform_from_bones;
	final_transform_from_bones_no_translation[3][0] = 0.0;
	final_transform_from_bones_no_translation[3][1] = 0.0;
	final_transform_from_bones_no_translation[3][2] = 0.0;

	float3 oNormale = mul(initial_n, final_transform_from_bones_no_translation);
	Output.Normale.xyz = normalize(oNormale);
	Output.Normale.w = 1.0;

	float3 oTangent = mul(initial_t, final_transform_from_bones_no_translation);
	Output.Tangent.xyz = normalize(oTangent);
	Output.Tangent.w = 1.0;

	float3 oBinormale = mul(initial_b, final_transform_from_bones_no_translation);
	Output.Binormale.xyz = normalize(oBinormale);
	Output.Binormale.w = 1.0;
    
    Output.TexCoord0 = Input.TexCoord0;

    if (Lights_Enabled.x > 0.0)
    {
        Output.Half0 = HalfVectorForLights(pos, Light0_Dir, matCam, matWorld);
    }
    else
    {        
        Output.Half0 = 0.0;
    }

    float4 PositionWV = mul(Input.Position, matWorldView);
    Output.Fog = clamp(0.0, 1.0, ComputeExp2Fog(PositionWV, Flags.y));

    return( Output );   
}
