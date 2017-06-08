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

#define v_flag0                     24
#define v_patch_translation         25
#define v_base_uv                   26
#define v_base_uv_global            27
#define v_viewer_pos                28


#define matLandPlacePatchLocalPos   29

#define v_landplacepatch_normale    33

#define v_landscape_control         40
#define v_seeds                     41

#define v_atmo_scattering_flag_0    42
#define v_atmo_scattering_flag_1    43
#define v_atmo_scattering_flag_2    44
#define v_atmo_scattering_flag_3    45
#define v_atmo_scattering_flag_4    46
#define v_atmo_scattering_flag_5    47
#define v_atmo_scattering_flag_6    48

#define v_flags_lights              50
#define v_ambient_color             51

#define v_light0_dir_local          52
#define v_light0_dir                53
#define v_light0_color              54

#define v_light1_dir_local          55
#define v_light1_dir                56
#define v_light1_color              57

#define v_light2_dir_local          58
#define v_light2_dir                59
#define v_light2_color              60


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

    float4 c0 : COLOR0;
    float4 c1 : COLOR1;

    float4 c0_1 : COLOR2;
    float4 c1_1 : COLOR3;

    float4 c0_2 : COLOR4;
    float4 c1_2 : COLOR5;

    float Fog : FOG;
};

#include "mat_input_constants.hlsl"
#include "fbm.hlsl"
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"
#include "atmospheric_scattering.hlsl"



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

    float4 ambient_color = vec[v_ambient_color];

    float4 light0_dir_local = vec[v_light0_dir_local];
    float4 light0_dir = vec[v_light0_dir];
    float4 light0_color = vec[v_light0_color];

    float4 light1_dir_local = vec[v_light1_dir_local];
    float4 light1_dir = vec[v_light1_dir];
    float4 light1_color = vec[v_light1_color];

    float4 light2_dir_local = vec[v_light2_dir_local];
    float4 light2_dir = vec[v_light2_dir];
    float4 light2_color = vec[v_light2_color];

    float4 atmo_scattering_flag_0 = vec[v_atmo_scattering_flag_0];
    float4 atmo_scattering_flag_1 = vec[v_atmo_scattering_flag_1];
    float4 atmo_scattering_flag_2 = vec[v_atmo_scattering_flag_2];
    float4 atmo_scattering_flag_3 = vec[v_atmo_scattering_flag_3];
    float4 atmo_scattering_flag_4 = vec[v_atmo_scattering_flag_4];
    float4 atmo_scattering_flag_5 = vec[v_atmo_scattering_flag_5];
    float4 atmo_scattering_flag_6 = vec[v_atmo_scattering_flag_6];


    float4 landplacepatch_normale = vec[v_landplacepatch_normale];


    //////////////////////////////////////////////////////////////////////


    float4 v_position;
    float4 v_position3;

    if (patch_translation.z > 0.0)
    {
        v_position3.xyz = Input.Position.xyz;
        v_position3.w = 1.0;

        float4x4 mWorldView;
        mWorldView = mat[matWorldView];

        float4x4 mProj;
        mProj = mat[matProj];

        float4 v2 = mul(v_position3, mWorldView);

        //////////////////////////////////////////////////////

        float4 v_init;
        
        v_init.xyz = Input.Position.xyz;
        v_init.w = 1.0;

        float4 v_local_pos = mul(v_init, mat[matLandPlacePatchLocalPos]);

        float4 v_local_pos_unit;
        
        v_local_pos_unit.xyz = normalize(v_local_pos.xyz);
        v_local_pos_unit.w = 1.0;

        float v_alt = ComputeVertexHeight(v_local_pos_unit, landscape_control.x, landscape_control.y, landscape_control.z, landscape_control.w, seeds.x, seeds.y, seeds.z, seeds.w);

        if (v_alt >= 0.0)
        {
            v2.xyz += landplacepatch_normale.xyz * v_alt;

        }

        //////////////////////////////////////////////////////
                
        v2[2] = -v2[2];
        Output.Position = mul(v2, mProj);

    }
    else
    {
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


        float4 v_position4 = mul(v_position3, mat[matWorldView]);
        float vertex_distance = sqrt(v_position4.x * v_position4.x + v_position4.y * v_position4.y + v_position4.z * v_position4.z);

        float viewer_alt = flag0.w * flag0.z;
        float horizon_limit = sqrt(viewer_alt * viewer_alt - flag0.z * flag0.z);

        float v_alt = 0.0;

        float4 global_uv = 0.0;
        global_uv.x = lerp(base_uv_global.x, base_uv_global.z, Input.TexCoord0.x);
        global_uv.y = lerp(base_uv_global.y, base_uv_global.w, Input.TexCoord0.y);
        //float v_factor = ComputeRiversFromTexture(TextureRivers, v_position2, global_uv, seeds.z, seeds.w);

        if (vertex_distance < 1.05 * horizon_limit)
        {
        	
            v_alt = ComputeVertexHeight(v_position2, landscape_control.x, landscape_control.y, landscape_control.z, landscape_control.w, seeds.x, seeds.y, seeds.z, seeds.w);
        //v_alt += ComputeCanyonsFromTexture(TextureCanyons, v_position2, global_uv, seeds.z, seeds.w);

            if (v_alt >= 0.0)
            {
          
			// seuls les vertex "non skirt" prennent en compte l'altitude calculee du vertex;
			// les vertex "skirt" ont toujours une altitude de zero

                if (Input.TexCoord0.z == 0.0)
                {
                    v_position3 *= (1.0 + ( /*v_factor * */(v_alt / flag0.z)));
                }
                else
                {
                    v_position3 *= (1.0 + (-100.0 / flag0.z));
                }
            }
        }


        v_position3.w = 1.0;

        Output.Position = mul(v_position3, mat[matWorldViewProjection]);
    }


    //Output.Position = mul(v_position3, mat[matWorldViewProjection]);
	
    Output.LODGlobalPatch_TexCoord = 0.0;
    Output.LODGlobalPatch_TexCoord.x = lerp(base_uv.x, base_uv.z, Input.TexCoord0.x);
    Output.LODGlobalPatch_TexCoord.y = lerp(base_uv.y, base_uv.w, Input.TexCoord0.y);

    Output.LODGlobalPatch_TexCoord.z = 0.0;
    Output.LODGlobalPatch_TexCoord.w = 0.0;

	// conserver aussi les coords textures originales du patch
    Output.UnitPatch_TexCoord = 0.0;
    Output.UnitPatch_TexCoord.x = Input.TexCoord0.x;
    Output.UnitPatch_TexCoord.y = Input.TexCoord0.y;
    Output.UnitPatch_TexCoord.z = patch_translation.z;
	
    Output.GlobalPatch_TexCoord = 0.0;
    Output.GlobalPatch_TexCoord.x = lerp(base_uv_global.x, base_uv_global.z, Input.TexCoord0.x);
    Output.GlobalPatch_TexCoord.y = lerp(base_uv_global.y, base_uv_global.w, Input.TexCoord0.y);
    ///////////////////////////////////////////////////

    Output.c0 = 0.0;
    Output.c1 = 0.0;
    Output.c0_1 = 0.0;
    Output.c1_1 = 0.0;
    Output.c0_2 = 0.0;
    Output.c1_2 = 0.0;

    Output.Fog = 0.0;

    return (Output);
}
