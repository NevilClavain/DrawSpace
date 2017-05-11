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
#define v_landscape_control         30
#define v_seeds                     31

#define v_atmo_scattering_flag_0    32
#define v_atmo_scattering_flag_1    33
#define v_atmo_scattering_flag_2    34
#define v_atmo_scattering_flag_3    35
#define v_atmo_scattering_flag_4    36
#define v_atmo_scattering_flag_5    37
#define v_atmo_scattering_flag_6    38

#define v_flags_lights              40
#define v_ambient_color             41

#define v_light0_dir_local          42
#define v_light0_dir                43
#define v_light0_color              44

#define v_light1_dir_local          45
#define v_light1_dir                46
#define v_light1_color              47

#define v_light2_dir_local          48
#define v_light2_dir                49
#define v_light2_color              50


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



double3 ProjectVectorToCube_double(int p_orientation, double3 p_vector)
{
    double3 res;

    if (0 == p_orientation) // front
    {
        res.x = p_vector.x;
        res.y = p_vector.y;
        res.z = p_vector.z;
    }
    else if (1 == p_orientation) // rear
    {
        res.x = -p_vector.x;
        res.y = p_vector.y;
        res.z = -p_vector.z;
    }
    else if (2 == p_orientation) // left
    {
        res.x = -p_vector.z;
        res.y = p_vector.y;
        res.z = p_vector.x;
    }
    else if (3 == p_orientation) // right
    {
        res.x = p_vector.z;
        res.y = p_vector.y;
        res.z = -p_vector.x;
    }
    else if (4 == p_orientation) // top
    {
        res.x = p_vector.x;
        res.y = p_vector.z;
        res.z = -p_vector.y;
    }
    else //if( 5 == p_orientation ) // bottom
    {
        res.x = p_vector.x;
        res.y = -p_vector.z;
        res.z = p_vector.y;
    }

    return res;
}

double3 CubeToSphere_double(double3 p_vector)
{
    double3 res;
    double xtemp = p_vector.x;
    double ytemp = p_vector.y;
    double ztemp = p_vector.z;

    res.x = xtemp * sqrt(1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp * 0.33333333 );
    res.y = ytemp * sqrt(1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp * 0.33333333 );
    res.z = ztemp * sqrt(1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp * 0.33333333 );

    return res;
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


    //////////////////////////////////////////////////////////////////////




    //float4 v_position;
    double4 v_position;

    double ray = flag0.z;

	// sidelenght scaling
    double sidelength = flag0.y;
    double3 Position = Input.Position;

    v_position.xyz = Position * sidelength * 0.5;

    double2 patch_translation_double = patch_translation.xy;


    v_position.xy = v_position.xy + patch_translation_double;
    v_position.z = 1.0;
    v_position.w = 1.0;
	
    double4 v_position2;
    v_position2.w = 1.0;
    v_position2.xyz = CubeToSphere_double(ProjectVectorToCube_double(flag0.x, v_position.xyz));

	// final scaling
    double4 v_position3;
    v_position3 = v_position2 * ray;
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

    Output.c0 = 0.0;
    Output.c1 = 0.0;
    Output.c0_1 = 0.0;
    Output.c1_1 = 0.0;
    Output.c0_2 = 0.0;
    Output.c1_2 = 0.0;

    Output.Fog = 0.0;

    return (Output);
}
