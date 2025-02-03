/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
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

float4x4 getTransformationMatrixForBone(int boneid, int array_begin_offset, float4 vectors[512])
{
    float4x4 bone_transform;

    int matrix_index = boneid * 3;    
    matrix_index += array_begin_offset;

    float4 col0 = vectors[matrix_index];
    float4 col1 = vectors[matrix_index + 1];
    float4 col2 = vectors[matrix_index + 2];

    bone_transform[0][0] = col0[0];
    bone_transform[1][0] = col0[1];
    bone_transform[2][0] = col0[2];
    bone_transform[3][0] = col0[3];

    bone_transform[0][1] = col1[0];
    bone_transform[1][1] = col1[1];
    bone_transform[2][1] = col1[2];
    bone_transform[3][1] = col1[3];

    bone_transform[0][2] = col2[0];
    bone_transform[1][2] = col2[1];
    bone_transform[2][2] = col2[2];
    bone_transform[3][2] = col2[3];

    bone_transform[0][3] = 0.0;
    bone_transform[1][3] = 0.0;
    bone_transform[2][3] = 0.0;
    bone_transform[3][3] = 1.0;

    transpose(bone_transform);

    return bone_transform;
}