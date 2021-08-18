/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                   
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

#define v_flag0                     24
#define v_patch_translation         25
#define v_base_uv                   26
#define v_base_uv_global            27
#define v_viewer_pos                28

#define v_mirror_flag				61


struct VS_INPUT
{
    float3 Position         : POSITION;
    float4 TexCoord0        : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position                 : SV_POSITION;

};

#include "mat_input_constants.hlsl"
#include "spherelod_commons.hlsl"
#include "landscapes.hlsl"


VS_OUTPUT vs_main(VS_INPUT Input)
{
    VS_OUTPUT Output;


    float4 flag0 = vec[v_flag0];
    float4 patch_translation = vec[v_patch_translation];    

    float4 viewer_pos = vec[v_viewer_pos];


	float4 mirror_flag = vec[v_mirror_flag];

    //////////////////////////////////////////////////////////////////////

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
    
    if (mirror_flag.x > 0.0)
    {
        // calculer normale et position du plan de reflection
        float4 rn = normalize(viewer_pos);
        float4 rp = rn * mirror_flag.y;
        float4 pos = mat[matWorld][3];
        rp += pos;

        Output.Position = reflected_vertex_pos(v_position3, rp, rn, matWorld, matView, matProj);
    }
    else
    {

        Output.Position = mul(v_position3, mat[matWorldViewProjection]);
    }
    return (Output);
}
