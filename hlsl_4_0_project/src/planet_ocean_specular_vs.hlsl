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


#define v_flags_lights              50

#define v_light0_dir                53
#define v_light1_dir                56
#define v_light2_dir                59



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
    float3 Normale                  : TEXCOORD3;
    float3 Half0                    : TEXCOORD4;
    float3 Half1                    : TEXCOORD5;
    float3 Half2                    : TEXCOORD6;
};

float4 HalfVector(float4 p_pos, float4 p_lightdir, float4x4 p_cammat, float4x4 p_worldmat)
{
    float4 CamPos;
    float4 Pos2;
    float3 nView;
    float3 nLight;

    CamPos[0] = p_cammat[3][0];
    CamPos[1] = p_cammat[3][1];
    CamPos[2] = p_cammat[3][2];
    CamPos[3] = 1.0;

    Pos2 = mul(p_pos, p_worldmat);
    nView = normalize(CamPos.xyz - Pos2.xyz);

    nLight = normalize(p_lightdir.xyz);

    float4 H;
    H.xyz = nLight + nView;
    H.w = 0.0;
    return normalize(H);
}


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

    float4 flags_lights = vec[v_flags_lights];

    float4 light0_dir = vec[v_light0_dir];
    float4 light1_dir = vec[v_light1_dir];
    float4 light2_dir = vec[v_light2_dir];


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
    
    if (flags_lights.y > 0.0)
    {
        Output.Half0 = HalfVector(v_position3, light0_dir, mat_Cam, mat_World);
    }
    else
    {
        Output.Half0 = 0;
    }

    if (flags_lights.z > 0.0)
    {
        Output.Half1 = HalfVector(v_position3, light1_dir, mat_Cam, mat_World);
    }
    else
    {
        Output.Half1 = 0;
    }

    if (flags_lights.w > 0.0)
    {
        Output.Half2 = HalfVector(v_position3, light2_dir, mat_Cam, mat_World);
    }
    else
    {
        Output.Half2 = 0;
    }

    // compute normale in world space
    float4x4 matWorldRot = mat_World;
    matWorldRot[3][0] = 0.0;
    matWorldRot[3][1] = 0.0;
    matWorldRot[3][2] = 0.0;

    float4 n2 = mul(v_position2, matWorldRot);
    Output.Normale = normalize(n2.xyz);

    return (Output);
}
