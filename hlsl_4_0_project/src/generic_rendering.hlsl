/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

float4 reflectedVertexPos(float4 p_vertex, float4 p_reflectorPos, float4 p_reflectorNormale, float4x4 p_matWorld, float4x4 p_matView, float4x4 p_matProj)
{
    float4 rvp;

    float4x4 id = 0;
    id[0][0] = 1.0;
    id[1][1] = 1.0;
    id[2][2] = -1.0;
    id[3][3] = 1.0;
    float4x4 matView2 = mul(p_matView, id);
    
    float4 worldPos = mul(p_vertex, p_matWorld);
    float4 worldPos2 = worldPos - p_reflectorPos;
    worldPos2.w = 1.0;
    
    float4 worldPosRefl;
    worldPosRefl.xyz = reflect(worldPos2.xyz, normalize(p_reflectorNormale.xyz));
    worldPosRefl.w = worldPos.w;

    float4 worldPos3 = worldPosRefl + p_reflectorPos;
    worldPos3.w = 1.0;

    float4 viewPos = mul(worldPos3, matView2);

    rvp = mul(viewPos, p_matProj);
    return rvp;
}

//pour lumieres diffuses : NORMAL transformee (sans les translations)
//dans repereworld
float4 TransformedNormaleForLights(float3 p_normale, float4x4 p_worldmat)
{
    float4x4 worldRot = p_worldmat;
    worldRot[0][3] = 0.0;
    worldRot[1][3] = 0.0;
    worldRot[2][3] = 0.0;
    return mul(p_normale, worldRot);
}

// compute half-vector for specular lights
float4 HalfVectorForLights(float4 p_pos, float4 p_lightdir, float4x4 p_cammat, float4x4 p_worldmat)
{
    float4 CamPos;
    float4 Pos2;
    float3 nView;
    float3 nLight;

    CamPos[0] = p_cammat[3][0];
    CamPos[1] = p_cammat[3][1];
    CamPos[2] = p_cammat[3][2];
    CamPos[3] = 1.0;

    Pos2 = mul(p_pos, p_worldmat);
    nView = normalize(CamPos.xyz - Pos2.xyz);

    nLight = normalize(-p_lightdir.xyz);

    float4 H;
    H.xyz = nLight + nView;
    H.w = 0.0;
    return H;
}


// compute pixel color combined from base color and light parameters
float4 computePixelColorFromLight(float4 p_basecolor,
                                    float4 p_ambient_color, bool p_light_enabled, float4 p_light_color, float4 p_light_dir, 
                                    float4 p_normale, bool p_spec_enabled, float p_spec_power, float p_spec_power_factor, float4 p_half0)
{
    float4 final_color = 0.0;
    float3 nNorm = normalize(p_normale);

    if (p_light_enabled)
    {
        float diff;
        float spec = 0.0;

        diff = dot(normalize(-p_light_dir.xyz), nNorm);
        if (p_spec_enabled)
        {
            spec = pow(clamp(dot(nNorm, normalize(p_half0.xyz)), 0.0, 1.0), p_spec_power);
        }

        final_color += max(0.0, diff * p_basecolor * p_light_color) + p_spec_power_factor * spec;
    }
    final_color += p_ambient_color * p_basecolor;
    return saturate(final_color);
}


float4 bump_vector_avg(float2 p_tex_coords, Texture2D p_txBump, SamplerState p_samBump, int p_texture_resol)
{
    float texel_size = 1.0 / (float)p_texture_resol;

    float2 left_coords = p_tex_coords;
    left_coords.x -= texel_size;

    float2 right_coords = p_tex_coords;
    right_coords.x += texel_size;

    float2 up_coords = p_tex_coords;
    up_coords.y -= texel_size;

    float2 down_coords = p_tex_coords;
    down_coords.y += texel_size;

    float4 bump_left = p_txBump.Sample(p_samBump, left_coords);
    float4 bump_right = p_txBump.Sample(p_samBump, right_coords);
    float4 bump_up = p_txBump.Sample(p_samBump, up_coords);
    float4 bump_down = p_txBump.Sample(p_samBump, down_coords);

    float4 bump_center = p_txBump.Sample(p_samBump, p_tex_coords).x;

    float4 res;

    res.xyz = normalize((bump_center.xyz + bump_left.xyz + bump_right.xyz + bump_up.xyz + bump_down.xyz) / 5.0);
    res.w = 1.0;
    return res;
}

float4 bump_bias_vector(float2 p_tex_coords, Texture2D p_txBump, SamplerState p_samBump, int p_texture_resol, float p_vector_bias)
{
    float texel_size = 1.0 / (float)p_texture_resol;

    float2 left_coords = p_tex_coords;
    left_coords.x -= texel_size;

    float2 right_coords = p_tex_coords;
    right_coords.x += texel_size;

    float2 up_coords = p_tex_coords;
    up_coords.y -= texel_size;

    float2 down_coords = p_tex_coords;
    down_coords.y += texel_size;


    float4 bump_left = p_txBump.Sample(p_samBump, left_coords);
    float4 bump_right = p_txBump.Sample(p_samBump, right_coords);
    float4 bump_up = p_txBump.Sample(p_samBump, up_coords);
    float4 bump_down = p_txBump.Sample(p_samBump, down_coords);

    float bump_center = p_txBump.Sample(p_samBump, p_tex_coords).x;

    float vector_bias = p_vector_bias;
    float3 vec_left;
    vec_left.x = -vector_bias;
    vec_left.y = 0.0;
    vec_left.z = (bump_left - bump_center);

    float3 vec_right;
    vec_right.x = vector_bias;
    vec_right.y = 0.0;
    vec_right.z = (bump_right - bump_center);

    float3 vec_up;
    vec_up.x = 0.0;
    vec_up.y = -vector_bias;
    vec_up.z = (bump_up - bump_center);

    float3 vec_down;
    vec_down.x = 0.0;
    vec_down.y = vector_bias;
    vec_down.z = (bump_down - bump_center);

    float3 vec1 = normalize(cross(vec_right, vec_down));
    float3 vec2 = normalize(cross(vec_down, vec_left));
    float3 vec3 = normalize(cross(vec_left, vec_up));
    float3 vec4 = normalize(cross(vec_up, vec_right));

    float4 avg; 
    
    avg.xyz = normalize(0.25 * (vec1 + vec2 + vec3 + vec4));
    avg.w = 1.0;

    return avg;
}

float ComputeExp2Fog(float4 worldViewPos, float density)
{
    float4 org;
   
    org.x = 0.0;
    org.y = 0.0;
    org.z = 0.0;
    org.w = 1.0;

    float4 d = -worldViewPos[2];
    return 1 / exp2(d * density);
   
   // retour a 0.0 -> brouillard au maximum
}

float4x4 GetTransformationMatrixForBone(int boneid, float4 vectors[512])
{
	float4x4 bone_transform;

	int matrix_index = boneid * 3;

	if (boneid <= 22)
	{
		// 1st set of 69 vectors triplets : from 31 to 99 
		matrix_index += 31;
	}
	else
	{
		// 2nd set of 132 vectors triplet : from 124 to 255 

		matrix_index -= 69;
		matrix_index += 124;
	}

	float4 col0 = vectors[matrix_index];
	float4 col1 = vectors[matrix_index+1];
	float4 col2 = vectors[matrix_index+2];

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
