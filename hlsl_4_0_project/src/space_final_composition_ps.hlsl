/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
Texture2D txOceanMask               : register(t3);
SamplerState SamplerOceanMask       : register(s3);



struct PS_INTPUT 
{
    float4 Position : SV_POSITION;
	float2 TexCoord0: TEXCOORD0;
};


float4 ps_main(PS_INTPUT input) : SV_Target
{
    float debug_mode = vec[2].x;
    float relative_alt = vec[0].x;

    
    float4 scene_color = 0.0;
    
    if (debug_mode == 0.0)
    {
        float4 mask;
        mask = txOceanMask.Sample(SamplerOceanMask, input.TexCoord0).xyzw;
        if (mask.y > 0.0)
        {

            if (relative_alt > 1.0)
            {
                ////////////// PLANET WATER RENDERING
                float4 basic_water_color = { 0.17, 0.36, 0.48, 1.0 };

                //float alt = mask.z;
                float water_color_transition_high = 1.0005; // relative alt
                float water_color_transition_low = 1.0001; // relative alt                
                float alt = clamp(0.0, 1.0, (relative_alt - water_color_transition_low) / (water_color_transition_high - water_color_transition_low));


                float2 bump_factor = txBump.Sample(SamplerBump, input.TexCoord0).xz;

                float2 mt = input.TexCoord0.xy + bump_factor;
                float2 mt2 = input.TexCoord0.xy + 0.25 * bump_factor;

                float4 refrac = txDiffuse.Sample(SamplerDiffuse, mt2);
                float3 mirror = txDiffuseMirror.Sample(SamplerDiffuseMirror, mt).rgb;

                float reflex_refrac_factor = mask.y;

                float light_luminance = mask.x;

                float3 detailed_water_color;
                detailed_water_color = basic_water_color * lerp(mirror, refrac, lerp(0.0, 1.0, reflex_refrac_factor));

                // transition between ocean "basic" color" and ocean details (reflexion + refraction)

                float3 pixel_color = lerp(detailed_water_color, basic_water_color, alt);

                float fog_factor = mask.w;
                float3 fog_color = { 0.45, 0.63, 0.78 };
                float3 fogged_color = saturate(lerp(fog_color, pixel_color, fog_factor));

                scene_color.rgb = light_luminance * fogged_color;
                ////////////// PLANET WATER RENDERING
            }
            else
            {
                scene_color.rgb = 1.0;
            }
        }
        else
        {
            scene_color.rgb = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0).rgb;
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


    scene_color.a = 1.0;
    return scene_color;
}
