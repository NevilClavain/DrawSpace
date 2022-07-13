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

// debug mode : 1
Texture2D txDiffuse                 : register(t0);
SamplerState SamplerDiffuse         : register(s0);

// debug mode : 2
Texture2D txDiffuseMirror           : register(t1);
SamplerState SamplerDiffuseMirror   : register(s1);

// debug mode : 4
Texture2D txBump                    : register(t2);
SamplerState SamplerBump            : register(s2);

// debug mode : 3
Texture2D txDiffuseRefrac           : register(t3);
SamplerState SamplerDiffuseRefrac   : register(s3);

Texture2D txNormales                : register(t4);
SamplerState SamplerNormales        : register(s4);

Texture2D txHalfVector              : register(t5);
SamplerState SamplerHalfVector      : register(s5);

struct PS_INTPUT 
{
    float4 Position : SV_POSITION;
	float2 TexCoord0: TEXCOORD0;
};

float4 ps_main(PS_INTPUT input) : SV_Target
{
    float debug_mode = vec[2].x;
    float4 scene_color = 0.0;

    if (debug_mode == 0.0)
    {
        scene_color = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0);
        if (scene_color.x == 1.0 && scene_color.y == 0.0 && scene_color.z == 1.0)
        {
            float3 surface_normale;

            surface_normale = vec[1].xyz;

            float2 bump_factor = txBump.Sample(SamplerBump, input.TexCoord0).xz;

            float spec_power = vec[3].x;

            float3 nNormale = normalize(surface_normale);

            float reflex_refrac_factor = txNormales.Sample(SamplerNormales, input.TexCoord0).b;

            float2 mt = input.TexCoord0.xy + bump_factor;
            float2 mt2 = input.TexCoord0.xy + 0.25 * bump_factor;

            float3 halfvector = txHalfVector.Sample(SamplerHalfVector, mt).rgb;

            float4 refrac = txDiffuseRefrac.Sample(SamplerDiffuseRefrac, mt2);
            float4 mirror = txDiffuseMirror.Sample(SamplerDiffuseMirror, mt);

            float4 color_mod = vec[0];

            float spec = pow(clamp(dot(nNormale, halfvector), 0.0, 1.0), spec_power);

            scene_color = saturate((color_mod * lerp(mirror, refrac, lerp(0.0, 0.99, reflex_refrac_factor))) + spec);
        }
    }
    else if (debug_mode == 1.0)
    {
        scene_color = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0);
    }

    else if (debug_mode == 2.0)
    {
        scene_color = txDiffuseMirror.Sample(SamplerDiffuseMirror, input.TexCoord0);
    }

    else if (debug_mode == 3.0)
    {
        scene_color = txDiffuseRefrac.Sample(SamplerDiffuseRefrac, input.TexCoord0);
    }

    else if (debug_mode == 4.0)
    {
        scene_color = txBump.Sample(SamplerBump, input.TexCoord0);
    }

    return scene_color;
}
