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


struct PS_INTPUT
{
    float4 Position : SV_POSITION;
    float4 aht : TEXCOORD0; // aht : altitude temperature humidity
};

#define v_flags6                    6

float4 ps_main(PS_INTPUT input) : SV_Target
{    
    float4 color = 0.0;

    float point_alt = input.aht.x;

    float color_temp = input.aht.y;
    float color_humidity = input.aht.z;

    color.x = color_temp;
    color.y = color_humidity;

    bool oceans_enabled = vec[v_flags6].x;
    
    if (oceans_enabled)
    {
        if (point_alt <= 0.0)
        {
            color.z = 1.0;
        }
    }   
    color.w = point_alt;

    return color;
}
