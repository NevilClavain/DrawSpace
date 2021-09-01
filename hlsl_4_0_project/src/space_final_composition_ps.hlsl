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


struct PS_INTPUT 
{
    float4 Position : SV_POSITION;
	float2 TexCoord0: TEXCOORD0;
};


float4 ps_main(PS_INTPUT input) : SV_Target
{
    float debug_mode = vec[2].x;
    float4 final_color;
    
    if (debug_mode == 0.0)
    {
        final_color.rgb = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0).rgb;
    }
    else if (debug_mode == 1.0)
    {
        final_color.rgb = txDiffuse.Sample(SamplerDiffuse, input.TexCoord0).rgb;
    }
    else if (debug_mode == 2.0)
    {
        final_color.rgb = txDiffuseMirror.Sample(SamplerDiffuseMirror, input.TexCoord0).rgb;
    }


    final_color.a = 1.0;

    return final_color;
}