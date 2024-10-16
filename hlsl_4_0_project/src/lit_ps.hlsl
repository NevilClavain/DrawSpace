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

Texture2D txColor           : register(t0);
SamplerState samColor       : register(s0);

Texture2D txBump            : register(t1);
SamplerState samBump        : register(s1);

Texture2D txMask            : register(t2);
SamplerState samMask        : register(s2);

Texture2D txEm              : register(t3);
SamplerState samEm          : register(s3);

struct PS_INTPUT 
{
    float4 Position     : SV_POSITION;
    float2 TexCoord0    : TEXCOORD0;
    float4 Normale      : TEXCOORD1;
    float4 Tangent      : TEXCOORD2;
    float4 Binormale    : TEXCOORD3;
    float4 Half0        : TEXCOORD4;
    float  Fog          : FOG;
};

#include "mat_input_constants.hlsl"
#include "generic_rendering.hlsl"

// Flags ->
//          
//          .x = specular power; if == 0.0 specular is deactivated
//          .y = bump mapping bias. if == 0.0, bump mapping is deactivated
//          .z = texture size
//          .w = specular mask mode

// base_color = (bf * texture) + ((1-bf) * input_color )

// pl = f(base_color, lights)


// em = self-emissive
// ab = lightabsorption
 
// (1-em) * (ab * pl) + (em * base_color)

// Flags2 -> 
//
//          .x = bump mapping normales input mod; 0.0 : input is heightmap; 1.0 : input is rgb(xyz) map


float4 ps_main(PS_INTPUT input) : SV_Target
{
    float4 final_color = 0.0;

    float4 flags = vec[7];
    float4 self_emissive = vec[8];
    float4 absorption = 1.0 - vec[9];
    float4 color = vec[10];
    float4 color_source = vec[11];
    float4 fog_color = vec[12];
    float4 flags2 = vec[13];

    float spec_power = flags.x;
    bool spec_enabled = false;

    bool spec_mask_mode = flags.w;

    float4 em_intensity = 0.0;
    if (self_emissive.x > 0.0 || self_emissive.y > 0.0 || self_emissive.z > 0.0)
    {
        em_intensity = txEm.Sample(samEm, input.TexCoord0);
    }

    if (spec_power > 0.0)
    {
        spec_enabled = true;
    }
    float spec_intensity = 1.0;

    
    if (spec_mask_mode)
    {
        float4 mask = txMask.Sample(samMask, input.TexCoord0);
        spec_intensity = mask.r;
    }
    
    int texture_size = flags.z;
    float bump_bias = flags.y;

    float4 Ambient_Color = vec[0];
    float4 Lights_Enabled = vec[1];
    float4 Light0_Color = vec[2];
    float4 Light0_Dir = vec[3];

    float4x4 mat_World = mat[matWorld_ps];

    float4x4 tbn_mat;

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

    if (bump_bias > 0.0)
    {
        float4 local_normale;

        if (flags2.x == 0.0)
        {
            local_normale = bump_bias_vector(input.TexCoord0, txBump, samBump, texture_size, bump_bias);
        }
        else
        {

            local_normale = bump_vector_avg(input.TexCoord0, txBump, samBump, texture_size);
        }       
        object_normale = mul(tbn_mat, local_normale);
    }
    else
    {
        object_normale = input.Normale;
    }

    float4 world_normale = TransformedNormaleForLights(object_normale, mat_World);

    float4 base_color = (color_source * txColor.Sample(samColor, input.TexCoord0)) + ((1.0 - color_source) * color);

    float4 pixel_light_color = 0.0;

    pixel_light_color += computePixelColorFromLight(base_color, Ambient_Color, (bool) Lights_Enabled.x, Light0_Color,
                                                Light0_Dir, world_normale, spec_enabled, spec_power, spec_intensity, input.Half0);

    final_color = ((1.0 - em_intensity * self_emissive) * (absorption * pixel_light_color)) + (em_intensity * self_emissive * base_color);

    final_color = saturate(lerp(fog_color, final_color, input.Fog));

    return saturate(final_color);
}
