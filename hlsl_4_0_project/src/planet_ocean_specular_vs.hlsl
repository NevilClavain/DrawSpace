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
#include "fbm.hlsl"
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"
#include "generic_rendering.hlsl"

#define v_flag0                     24
#define v_patch_translation         25
#define v_base_uv                   26
#define v_base_uv_global            27
#define v_viewer_pos                28

#define v_landscape_control         40
#define v_seeds                     41

#define v_atmo_scattering_flag_0    42
#define v_atmo_scattering_flag_5    47


#define v_light0_dir_local          52
#define v_light0_dir                53
#define v_light0_color              54

struct VS_INPUT
{
    float3 Position         : POSITION;
    float4 TexCoord0        : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position                 : SV_POSITION;
    float4 LODGlobalPatch_TexCoord  : TEXCOORD0;
    float4 UnitPatch_TexCoord       : TEXCOORD1;
    float4 GlobalPatch_TexCoord     : TEXCOORD2;
    float3 Half0                    : TEXCOORD3;
    float3 Normale                  : TEXCOORD4;
};




VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;


    float4 flag0 = vec[v_flag0];
    float4 patch_translation = vec[v_patch_translation];    
    float4 base_uv = vec[v_base_uv];
    float4 base_uv_global = vec[v_base_uv_global];
    float4 viewer_pos = vec[v_viewer_pos];
    float4 landscape_control = vec[v_landscape_control];
    float4 seeds = vec[v_seeds];

    float4 light0_dir = vec[v_light0_dir];


    float4 atmo_scattering_flag_0 = vec[v_atmo_scattering_flag_0];
    float4 atmo_scattering_flag_5 = vec[v_atmo_scattering_flag_5];

    float4x4 mat_World = mat[matWorld];
    float4x4 mat_Cam = mat[matCam];


    //////////////////////////////////////////////////////////////////////

    float4 v_position;
    float v_alt = 0.0;

    float relative_alt = flag0.w;

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

    // conserver aussi les coords textures originales du patch
    Output.UnitPatch_TexCoord = 0.0;
    Output.UnitPatch_TexCoord.x = Input.TexCoord0.x;
    Output.UnitPatch_TexCoord.y = Input.TexCoord0.y;

    Output.GlobalPatch_TexCoord = 0.0;
    Output.GlobalPatch_TexCoord.x = lerp(base_uv_global.x, base_uv_global.z, Input.TexCoord0.x);
    Output.GlobalPatch_TexCoord.y = lerp(base_uv_global.y, base_uv_global.w, Input.TexCoord0.y);
    ///////////////////////////////////////////////////

    // compute half vector
    
    float3 nLight = normalize(light0_dir.xyz);

    float4 Pos2 = mul(v_position3, mat_World);

    float4 CamPos;
    CamPos[0] = mat_Cam[3][0];
    CamPos[1] = mat_Cam[3][1];
    CamPos[2] = mat_Cam[3][2];
    CamPos[3] = 1.0;

    float3 nView = normalize(CamPos.xyz - Pos2.xyz);

    float3 H;
    H.xyz = nLight + nView;

    Output.Half0 = normalize(H);

    // compute normale in world space
    float4x4 matWorldRot = mat_World;
    matWorldRot[3][0] = 0.0;
    matWorldRot[3][1] = 0.0;
    matWorldRot[3][2] = 0.0;

    float4 n2 = mul(v_position2, matWorldRot);
    Output.Normale = normalize(n2.xyz);

    return (Output);
}
