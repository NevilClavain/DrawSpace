/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

struct PS_INTPUT
{
    float4 Position     : SV_POSITION;
    float2 TexCoord0    : TEXCOORD0;
    float4 Normale      : TEXCOORD1;
    float4 Tangent      : TEXCOORD2;
    float4 Binormale    : TEXCOORD3;
    float4 Half0        : TEXCOORD4;
    float3 LocalePos    : TEXCOORD5;
    float  Fog : FOG;
};

#include "mat_input_constants.hlsl"
#include "fbm.hlsl"
#include "generic_rendering.hlsl"

// Flags ->
//          
//          .x = specular power; if == 0.0 specular is deactivated
//          .y = bump mapping bias. if == 0.0, bump mapping is deactivated
//          .z = texture size
//          .w = specular mask mode

// Flags2 -> 
//
//          .x = bump mapping normales input mod; 0.0 : input is heightmap; 1.0 : input is rgb(xyz) map


float4 ps_main(PS_INTPUT input) : SV_Target
{
    float4 final_color = 0.0;

    float4 flags = vec[7];
    float4 fog_color = vec[12];
    float4 flags2 = vec[13];

    float spec_power = flags.x;
    bool spec_enabled = false;

    int texture_size = flags.z;
    float bump_bias = 7.0; //flags.y;

    float4 Ambient_Color = vec[0];
    float4 Lights_Enabled = vec[1];
    float4 Light0_Color = vec[2];
    float4 Light0_Dir = vec[3];

    float4x4 mat_World = mat[matWorld_ps];

    float4x4 tbn_mat;

    float4 vpos;
    vpos.xyz = 0.9 * input.LocalePos;
    //vpos.xyz = input.LocalePos.xyz;
    
    vpos.w = 1.0;


    tbn_mat[0][0] = input.Tangent[0];
    tbn_mat[1][0] = input.Tangent[1];
    tbn_mat[2][0] = input.Tangent[2];
    tbn_mat[3][0] = 0.0;

    tbn_mat[0][1] = input.Binormale[0];
    tbn_mat[1][1] = input.Binormale[1];
    tbn_mat[2][1] = input.Binormale[2];
    tbn_mat[3][1] = 0.0;

    tbn_mat[0][2] = input.Normale[0];
    tbn_mat[1][2] = input.Normale[1];
    tbn_mat[2][2] = input.Normale[2];
    tbn_mat[3][2] = 0.0;

    tbn_mat[0][3] = 0.0;
    tbn_mat[1][3] = 0.0;
    tbn_mat[2][3] = 0.0;
    tbn_mat[3][3] = 1.0;

    transpose(tbn_mat);

    float4 object_normale;


    


    float4 vpos_up = vpos;
    float4 vpos_down = vpos;
    float4 vpos_left = vpos;
    float4 vpos_right = vpos;

    float step = 1.0;

    vpos_up.z += step;
    vpos_down.z -= step;

    vpos_left.x -= step;
    vpos_right.x += step;



    float color_res = iqTurbulence(1.0 * input.LocalePos.xyz, 3, 2.0, 0.46);

    float color_res_2 = iqTurbulence(0.05 * input.LocalePos.xyz, 3, 3.0, 0.20);
    

    float3 color1 = { 0.76, 0.72, 0.53 };
    float3 color2 = { 0.70, 0.60, 0.44 };
    float3 color3 = { 0.18, 0.40, 0.04 };



    float4 base_color;
    base_color.xyz = lerp( lerp(color1, color2, saturate(color_res)), color3, saturate(color_res_2));
    base_color.w = 1.0;

    /*
    base_color.x = 0.76;
    base_color.y = 0.72;
    base_color.z = 0.53;
    base_color.w = 1.0;
    */



    // SET 3
    
    float res = Fractal_fBm_wombat_perlin(vpos.xyz, 4, 2.0, 0.46, 0.0, 344.8, 890);
    float res_up = Fractal_fBm_wombat_perlin(vpos_up.xyz, 4, 2.0, 0.46, 0.0, 344.8, 890);
    float res_down = Fractal_fBm_wombat_perlin(vpos_down.xyz, 4, 2.0, 0.46, 0.0, 344.8, 890);
    float res_right = Fractal_fBm_wombat_perlin(vpos_right.xyz, 4, 2.0, 0.46, 0.0, 344.8, 890);
    float res_left = Fractal_fBm_wombat_perlin(vpos_left.xyz, 4, 2.0, 0.46, 0.0, 344.8, 890);
    
    
    

    //
    float4 local_normale;
    local_normale = bump_bias_vector_from_height_values(res, res_left, res_right, res_up, res_down, bump_bias);
    //local_normale.xyz = input.Normale.xyz;
    //local_normale.w = 1.0;
    //

    
    object_normale = mul(tbn_mat, local_normale);

    float4 world_normale = TransformedNormaleForLights(object_normale, mat_World);





    float4 pixel_light_color = 0.0;

    pixel_light_color += computePixelColorFromLight(base_color, Ambient_Color, (bool)Lights_Enabled.x, Light0_Color,
                                                Light0_Dir, world_normale, 0, 0, 0, input.Half0);

    final_color = pixel_light_color;

    final_color = saturate(lerp(fog_color, final_color, input.Fog));

    return saturate(final_color);
}
