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
        scene_color.rgb = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0).rgb;
        if (scene_color.x == 1.0 && scene_color.y == 0.0 && scene_color.z == 1.0)
        {
            float2 mt = input.TexCoord0.xy;
            float3 mirror = txDiffuseMirror.Sample(SamplerDiffuseMirror, mt).rgb;

            scene_color.rgb = mirror;
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
    else if (debug_mode == 4.0)
    {
        scene_color.rgb = txBump.Sample(SamplerBump, input.TexCoord0).rgb;
    }


    scene_color.a = 1.0;
    return scene_color;
}
