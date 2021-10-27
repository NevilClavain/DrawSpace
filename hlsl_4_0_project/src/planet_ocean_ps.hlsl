/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                  
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
    float4 Position                 : SV_POSITION;
    float4 TexCoord0                : TEXCOORD0;
};

#define v_flags                     0

float4 ps_main(PS_INTPUT input) : SV_Target
{
    float v_alt = input.TexCoord0.x;

    float4 flags = vec[v_flags];

    float water_color_transition_high = 1.0005; // relative alt
    float water_color_transition_low = 1.0001; // relative alt

    float relative_alt = flags.x;
    float alt = clamp(0.0, 1.0, (relative_alt - water_color_transition_low) / (water_color_transition_high - water_color_transition_low));

    float surface_dot_delta = input.TexCoord0.y;

    float4 water_color = { 0.0, surface_dot_delta, alt, 0.0 };
    if (v_alt > 0.0)
    {
        clip(-1.0);
    }    
    return water_color;
}
