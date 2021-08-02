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


Texture2D txDiffuse                 : register(t0);
SamplerState SamplerDiffuse         : register(s0);

Texture2D txDiffuseMirror           : register(t1);
SamplerState SamplerDiffuseMirror   : register(s1);

Texture2D txBump                    : register(t2);
SamplerState SamplerBump            : register(s2);

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
    float4 scene_color = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0);                 
    if( scene_color.x == 1.0 && scene_color.y == 0.0 && scene_color.z == 1.0 )
    {
        float3 surface_normale;
        
        surface_normale = vec[1].xyz;
        

        float2 bump_factor = txBump.Sample(SamplerBump, input.TexCoord0).xz;

        float spec_power = 10.0;
        float k_specular = 0.33;

        surface_normale += normalize(txBump.Sample(SamplerBump, input.TexCoord0).xzy);
        
        float3 nNormale = normalize(surface_normale);

        float reflex_refrac_factor = txNormales.Sample(SamplerNormales, input.TexCoord0).b;

        float3 halfvector = txHalfVector.Sample(SamplerHalfVector, input.TexCoord0).rgb;

        float2 mt = input.TexCoord0.xy + bump_factor;
        float2 mt2 = input.TexCoord0.xy + 0.25 * bump_factor;

        float4 refrac = txDiffuseRefrac.Sample(SamplerDiffuseRefrac, mt2);
        float4 mirror = txDiffuseMirror.Sample(SamplerDiffuseMirror, mt);

        float4 color_mod = vec[0];

        float spec = pow(clamp(dot(nNormale, halfvector), 0.0, 1.0), spec_power);
     
        scene_color = saturate((color_mod * lerp(mirror, refrac, lerp(0.0, 0.99, reflex_refrac_factor))) + (k_specular * spec));

    }   
    return scene_color;
}
