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


struct PS_INTPUT 
{
    float4 Position : SV_POSITION;
	float2 TexCoord0: TEXCOORD0;
};


float4 ps_main(PS_INTPUT input) : SV_Target
{   
    float reflex_refrac_factor = txNormales.Sample(SamplerNormales, input.TexCoord0).b;
    float4 scene_color = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0);

    float4 color_mod = { 0.5, 0.68, 0.95, 1.0 };
          
    if( scene_color.x == 1.0 && scene_color.y == 0.0 && scene_color.z == 1.0 )
    {
        float2 mt = input.TexCoord0.xy + txBump.Sample(SamplerBump, input.TexCoord0).xy;
        float2 mt2 = input.TexCoord0.xy + 0.05 * txBump.Sample(SamplerBump, input.TexCoord0).xy;

        float4 refrac = txDiffuseRefrac.Sample(SamplerDiffuseRefrac, mt2);
        float4 mirror = txDiffuseMirror.Sample(SamplerDiffuseMirror, mt);
        
        scene_color = color_mod * lerp(mirror, refrac, lerp(0.4, 0.99, reflex_refrac_factor));
    }    
    return scene_color;
        
}
