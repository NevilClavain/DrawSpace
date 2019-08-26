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
   float4 Bones         : TEXCOORD6;
   float4 Weights       : TEXCOORD7;
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
		int4 boneIds;

		boneIds[0] = (int)Input.Bones.x;
		boneIds[1] = (int)Input.Bones.y;
		boneIds[2] = (int)Input.Bones.z;
		boneIds[3] = (int)Input.Bones.w;

		float4 weights = Input.Weights;

		// reconstituer les 4 matrices

		float4x4 mat0 = GetTransformationMatrixForBone(boneIds[0], bones_0, bones_1);
		float4x4 mat1 = GetTransformationMatrixForBone(boneIds[1], bones_0, bones_1);
		float4x4 mat2 = GetTransformationMatrixForBone(boneIds[2], bones_0, bones_1);
		float4x4 mat3 = GetTransformationMatrixForBone(boneIds[3], bones_0, bones_1);

		final_transform_from_bones = mat0 * weights[0] +
			mat1 * weights[1] +
			mat2 * weights[2] +
			mat3 * weights[3];
	}

	VS_OUTPUT Output;
	float4 initial_pos;

	initial_pos.xyz = Input.Position;
	initial_pos.w = 1.0;

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

    Output.Normale.xyz = Input.Normal;
    Output.Normale.w = 1.0;

    Output.Tangent.xyz = Input.Tangent;
    Output.Tangent.w = 1.0;

    Output.Binormale.xyz = Input.Binormale;
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
