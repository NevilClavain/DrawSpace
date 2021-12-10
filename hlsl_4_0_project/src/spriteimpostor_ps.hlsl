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


Texture2D txDiffuse : register(t0);
SamplerState sam : register(s0);

struct PS_INTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord0 : TEXCOORD0;
};


float4 ps_main(PS_INTPUT input) : SV_Target
{   
    float4 flags        = vec[0];
    float4 color        = vec[1];
    float4 keycolor     = vec[2];
    float4 ret_color;

    float4 texel = txDiffuse.Sample(sam, input.TexCoord0);

    
    if (flags.x > 0.0)
    {
        ret_color = color;
    }
    else
    {
        ret_color = color * texel;
    }
    

    ret_color = texel;

    
    if (flags.y > 0.0)
    {
        //if (abs(texel.x - keycolor.x) < 0.01 && abs(texel.y - keycolor.y) < 0.01 && abs(texel.z - keycolor.z) < 0.01)
        if (texel.x==keycolor.x && texel.y==keycolor.y && texel.z==keycolor.z)
        {
            clip(-1.0);
        }
    }
    

    return ret_color;
}














