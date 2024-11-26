/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

float computeExp2Fog(float depth, float density)
{
    float4 d = abs(depth);
    return 1 / exp2(d * density);
}

float4 fractal_texture(Texture2D tex, SamplerState sam, float2 uv, float depth)
{
    float LOD = log(depth);
    float LOD_floor = floor(LOD);
    float LOD_fract = LOD - LOD_floor;
    
    float2 uv1 = uv / exp(LOD_floor - 1.0);
    float2 uv2 = uv / exp(LOD_floor + 0.0);
    float2 uv3 = uv / exp(LOD_floor + 1.0);
    
    float4 tex0 = tex.Sample(sam, uv1);
    float4 tex1 = tex.Sample(sam, uv2);
    float4 tex2 = tex.Sample(sam, uv3);
    
    return (tex1 + lerp(tex0, tex2, LOD_fract)) * 0.5;
}

