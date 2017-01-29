/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

cbuffer legacyargs : register(b0)
{
    float4 vec[512];
    Matrix mat[512];
};

#include "mat_input_constants.hlsl"

#define v_flag0                   24
#define v_patch_translation       25
#define v_base_uv                 26
#define v_base_uv_global          27



struct VS_INPUT
{
    float3 Position : POSITION;
    float4 TexCoord0 : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position                 : SV_POSITION;
    
    float4 LODGlobalPatch_TexCoord  : TEXCOORD0;
    float4 UnitPatch_TexCoord       : TEXCOORD1;
    float4 GlobalPatch_TexCoord     : TEXCOORD2;
};

#include "mat_input_constants.hlsl"
#include "spherelod_commons.hlsl"

VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;

    float4 flag0 = vec[v_flag0];
    float4 patch_translation = vec[v_patch_translation];    
    float4 base_uv = vec[v_base_uv];
    float4 base_uv_global = vec[v_base_uv_global];

    float4 v_position;

	// sidelenght scaling

    v_position.xyz = Input.Position * flag0.y / 2.0;
    v_position.xy = v_position.xy + patch_translation.xy;
    v_position.z = 1.0;
    v_position.w = 1.0;
	
    float4 v_position2;
    v_position2.w = 1.0;
    v_position2.xyz = CubeToSphere(ProjectVectorToCube(flag0.x, v_position.xyz));

	// final scaling
    float4 v_position3;
    v_position3 = v_position2 * flag0.z;
    v_position3.w = 1.0;
    

    Output.Position = mul(v_position3, mat[matWorldViewProjection]);
	
    Output.LODGlobalPatch_TexCoord = 0.0;
    Output.LODGlobalPatch_TexCoord.x = lerp(base_uv.x, base_uv.z, Input.TexCoord0.x);
    Output.LODGlobalPatch_TexCoord.y = lerp(base_uv.y, base_uv.w, Input.TexCoord0.y);

    Output.LODGlobalPatch_TexCoord.z = 0.0;
    Output.LODGlobalPatch_TexCoord.w = 0.0;

	// conserver aussi les coords textures originales du patch
    Output.UnitPatch_TexCoord = 0.0;
    Output.UnitPatch_TexCoord.x = Input.TexCoord0.x;
    Output.UnitPatch_TexCoord.y = Input.TexCoord0.y;

	
    Output.GlobalPatch_TexCoord = 0.0;
    Output.GlobalPatch_TexCoord.x = lerp(base_uv_global.x, base_uv_global.z, Input.TexCoord0.x);
    Output.GlobalPatch_TexCoord.y = lerp(base_uv_global.y, base_uv_global.w, Input.TexCoord0.y);
    ///////////////////////////////////////////////////

    return (Output);
}
