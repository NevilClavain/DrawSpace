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

#include "spherelod_commons.hlsl"

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
Texture2D txOceanMask               : register(t3);
SamplerState SamplerOceanMask       : register(s3);

// debug mode : 5
Texture2D txOceanNormales           : register(t4);
SamplerState SamplerOceanNormales   : register(s4);

struct PS_INTPUT 
{
    float4 Position : SV_POSITION;
	float2 TexCoord0: TEXCOORD0;
};


float4 ps_main(PS_INTPUT input) : SV_Target
{
    float debug_mode = vec[2].x;
    float relative_alt = vec[0].x;
    float underwater_lightfactor = vec[1].x;
    bool oceans_enabled = vec[1].y;

    
    float4 scene_color = 0.0;
    float4 basic_water_color = { 1.0, 1.0, 1.0, 1.0 };
    
    if (debug_mode == 0.0)
    {
        float4 main_color = 0.0;
        main_color.rgba = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0).rgba;

        float4 mask;
        mask = txOceanMask.Sample(SamplerOceanMask, input.TexCoord0).xyzw;
        if (mask.y > 0.0 && main_color.w != SPHERELOD_FOLIAGE_ID)
        {

            ////////////// PLANET WATER RENDERING

            //float water_color_transition_high = 1.0005; // relative alt
            //float water_color_transition_low = 1.0001; // relative alt                
            //float alt = clamp(0.0, 1.0, (relative_alt - water_color_transition_low) / (water_color_transition_high - water_color_transition_low));


            float2 bump_factor = txBump.Sample(SamplerBump, input.TexCoord0).xz;

            float2 mt = input.TexCoord0.xy + bump_factor;
            float2 mt2 = input.TexCoord0.xy + 0.25 * bump_factor;

            
            float light_luminance = mask.x;
            float3 detailed_water_color;
            float spec_gain = 1.5;

            float refraction_gain = 2.0;


            float4 refrac = txDiffuse.Sample(SamplerDiffuse, mt2);

            float spec = spec_gain * saturate(txOceanNormales.Sample(SamplerOceanNormales, mt).rgb);

            if (relative_alt > 1.0)
            {
                float reflex_refrac_factor = saturate(mask.y * refraction_gain);
                float3 mirror = txDiffuseMirror.Sample(SamplerDiffuseMirror, mt).rgb;
                detailed_water_color = (basic_water_color * lerp(mirror, refrac, lerp(0.0, 1.0, reflex_refrac_factor))) + spec;
            }
            else
            {
                detailed_water_color = basic_water_color * refrac;
            }

            // transition between ocean "basic" color" and ocean details (reflexion + refraction)
            //float3 pixel_color = lerp(detailed_water_color, basic_water_color, alt);



            float3 pixel_color = detailed_water_color;

            float fog_factor = mask.w;
            float3 fog_color;

            //float3 fog_color = { 0.45, 0.63, 0.78 };
            if (relative_alt > 1.0)
            {
                // over the water : fog color is the same as the one used for planet ground
                fog_color.r = 0.45;
                fog_color.g = 0.63;
                fog_color.b = 0.78;
            }
            else
            {
                // underwater : fog color is water color
                fog_color.rgb = basic_water_color.rgb;
            }

            float3 fogged_color = saturate(lerp(fog_color, pixel_color, fog_factor));

            scene_color.rgb = light_luminance * fogged_color;
            ////////////// PLANET WATER RENDERING

        }
        else
        {
            if (relative_alt > 1.0 || !oceans_enabled)
            {
                scene_color.rgb = main_color.rgb;
            }
            else
            {                
                // if underwater
                if (main_color.a < SPHERELOD_GROUND_ID)
                {
                    //if pixel from other than planet ground (clouds, atmo, spacebox, ship, objects etc...)

                    if (mask.z > 0.0)
                    {
                        // here, objects that shall not be rendered with ocean background uninque color
                        // i.e object that are underwater : ship that dived underwater, underwater buildings, etc...
                        scene_color.rgb = main_color.rgb;
                    }
                    else
                    {
                        // here, object that shall be rendered with ocean background uninque color (deep blue...)
                        scene_color.rgb = underwater_lightfactor * basic_water_color.rgb;
                    }                    
                }
                else
                {                
                    //pixel from planet ground (identified by alpha == SPHERELOD_GROUND_ID)
                    scene_color.rgb = main_color.rgb;
                }
            }
        }
    }
    else if (debug_mode == 1.0)
    {
        scene_color.rgb = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0).rgb;
    }
    else if (debug_mode == 2.0)
    {
        scene_color.rgb = txDiffuseMirror.Sample(SamplerDiffuseMirror, input.TexCoord0).rgb;
    }
    else if (debug_mode == 3.0)
    {
        scene_color.rgb = txOceanMask.Sample(SamplerOceanMask, input.TexCoord0).rgb;
    }

    else if (debug_mode == 4.0)
    {
        scene_color.rgb = txBump.Sample(SamplerBump, input.TexCoord0).rgb;
    }

    else if (debug_mode == 5.0)
    {
        scene_color.rgb = txOceanNormales.Sample(SamplerOceanNormales, input.TexCoord0).rgb;
    }

    scene_color.a = 1.0;
    return scene_color;
}
