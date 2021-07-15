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


// genere un vecteur de bump mapping, c a d un vecteur biaise par le relief autour du center_point courant
float3 compute_terrain_bump_vector(float p_point_alt, int p_texture_resol, Texture2D p_HTMap_Texture, SamplerState p_HTMap_Texture_Sampler, float2 p_tex_coords, float p_vector_bias)
{
    float3 avg;

    float texel_size = 1.0 / p_texture_resol;

    float2 left_coords = p_tex_coords;
    left_coords.x -= texel_size;

    float2 right_coords = p_tex_coords;
    right_coords.x += texel_size;

    float2 up_coords = p_tex_coords;
    up_coords.y -= texel_size;

    float2 down_coords = p_tex_coords;
    down_coords.y += texel_size;

    float4 temp_humidity_left = p_HTMap_Texture.Sample(p_HTMap_Texture_Sampler, left_coords); //tex2D(p_htmap_texture, left_coords);
    float4 temp_humidity_right = p_HTMap_Texture.Sample(p_HTMap_Texture_Sampler, right_coords);//tex2D(p_htmap_texture, right_coords);
    float4 temp_humidity_up = p_HTMap_Texture.Sample(p_HTMap_Texture_Sampler, up_coords); //tex2D(p_htmap_texture, up_coords);
    float4 temp_humidity_down = p_HTMap_Texture.Sample(p_HTMap_Texture_Sampler, down_coords); //tex2D(p_htmap_texture, down_coords);



    float point_alt_center = p_point_alt;

    float point_alt_left = temp_humidity_left.w;
    float point_alt_right = temp_humidity_right.w;
    float point_alt_up = temp_humidity_up.w;
    float point_alt_down = temp_humidity_down.w;

	//float real_texel_size = 100.0; // controle de "l'intensit�" du relief bump mapping

    float3 vec_left;
    vec_left.x = -p_vector_bias;
    vec_left.y = 0.0;
    vec_left.z = (point_alt_left - point_alt_center);

    float3 vec_right;
    vec_right.x = p_vector_bias;
    vec_right.y = 0.0;
    vec_right.z = (point_alt_right - point_alt_center);

    float3 vec_up;
    vec_up.x = 0.0;
    vec_up.y = -p_vector_bias;
    vec_up.z = (point_alt_up - point_alt_center);

    float3 vec_down;
    vec_down.x = 0.0;
    vec_down.y = p_vector_bias;
    vec_down.z = (point_alt_down - point_alt_center);

	
    float3 vec1 = normalize(cross(vec_right, vec_down));
    float3 vec2 = normalize(cross(vec_down, vec_left));
    float3 vec3 = normalize(cross(vec_left, vec_up));
    float3 vec4 = normalize(cross(vec_up, vec_right));

    avg = normalize(0.25 * (vec1 + vec2 + vec3 + vec4));

    return avg;
}

// commenté provisoirement car il y a du hlsl 3.0 qui traine (sampler2D -> à remplacer par SampleState
/*
float3 compute_water_bump_vector(int p_texture_resol, sampler2D p_water_bump_texture, float2 p_tex_coords, float p_vector_bias)
{
    float3 avg = 0.0;

    float texel_size = 1.0 / p_texture_resol;

    float2 left_coords = p_tex_coords;
    left_coords.x -= texel_size;

    float2 right_coords = p_tex_coords;
    right_coords.x += texel_size;

    float2 up_coords = p_tex_coords;
    up_coords.y -= texel_size;

    float2 down_coords = p_tex_coords;
    down_coords.y += texel_size;

    float4 wb_left = tex2D(p_water_bump_texture, left_coords);
    float4 wb_right = tex2D(p_water_bump_texture, right_coords);
    float4 wb_up = tex2D(p_water_bump_texture, up_coords);
    float4 wb_down = tex2D(p_water_bump_texture, down_coords);
    float4 wb_center = tex2D(p_water_bump_texture, p_tex_coords);

    float3 vec_left;
    vec_left.x = -p_vector_bias;
    vec_left.y = 0.0;
    vec_left.z = (wb_left.x - wb_center.x);

    float3 vec_right;
    vec_right.x = p_vector_bias;
    vec_right.y = 0.0;
    vec_right.z = (wb_right.x - wb_center.x);

    float3 vec_up;
    vec_up.x = 0.0;
    vec_up.y = -p_vector_bias;
    vec_up.z = (wb_up.x - wb_center.x);

    float3 vec_down;
    vec_down.x = 0.0;
    vec_down.y = p_vector_bias;
    vec_down.z = (wb_down.x - wb_center.x);

    float3 vec1 = normalize(cross(vec_right, vec_down));
    float3 vec2 = normalize(cross(vec_down, vec_left));
    float3 vec3 = normalize(cross(vec_left, vec_up));
    float3 vec4 = normalize(cross(vec_up, vec_right));

    avg = normalize(0.25 * (vec1 + vec2 + vec3 + vec4));

    return avg;
}
*/

float3 compute_clouds_bump_vector(int p_texture_w_resol, int p_texture_h_resol, Texture2D p_bump_texture, SamplerState p_bump_texture_Sampler, float2 p_tex_coords, float p_vector_bias)
{
    float3 avg = 0.0;

    float texel_size_w = 1.0 / p_texture_w_resol;
    float texel_size_h = 1.0 / p_texture_h_resol;

    float2 left_coords = p_tex_coords;
    left_coords.x -= texel_size_w;

    float2 right_coords = p_tex_coords;
    right_coords.x += texel_size_w;

    float2 up_coords = p_tex_coords;
    up_coords.y -= texel_size_h;

    float2 down_coords = p_tex_coords;
    down_coords.y += texel_size_h;

    float4 wb_left = p_bump_texture.Sample(p_bump_texture_Sampler, left_coords);
    float4 wb_right = p_bump_texture.Sample(p_bump_texture_Sampler, right_coords);
    float4 wb_up = p_bump_texture.Sample(p_bump_texture_Sampler, up_coords);
    float4 wb_down = p_bump_texture.Sample(p_bump_texture_Sampler, down_coords);
    float4 wb_center = p_bump_texture.Sample(p_bump_texture_Sampler, p_tex_coords);


    float3 vec_left;
    vec_left.x = -p_vector_bias;
    vec_left.y = 0.0;
    vec_left.z = (wb_left.x - wb_center.x);

    float3 vec_right;
    vec_right.x = p_vector_bias;
    vec_right.y = 0.0;
    vec_right.z = (wb_right.x - wb_center.x);

    float3 vec_up;
    vec_up.x = 0.0;
    vec_up.y = -p_vector_bias;
    vec_up.z = (wb_up.x - wb_center.x);

    float3 vec_down;
    vec_down.x = 0.0;
    vec_down.y = p_vector_bias;
    vec_down.z = (wb_down.x - wb_center.x);

    float3 vec1 = normalize(cross(vec_right, vec_down));
    float3 vec2 = normalize(cross(vec_down, vec_left));
    float3 vec3 = normalize(cross(vec_left, vec_up));
    float3 vec4 = normalize(cross(vec_up, vec_right));

    avg = normalize(0.25 * (vec1 + vec2 + vec3 + vec4));

    return avg;
}


float2 compute_sampling_params(float p_param, float p_interval)
{
    float2 res;
	
    float t_offset = p_interval * floor(p_param / p_interval);

    float w2 = (p_param - t_offset) / p_interval;

    res.x = w2;
    res.y = t_offset;

    return res;
}

float4 splatting_color(float4 p_textcoords, float p_temperature, float p_humidity, float p_splatting_atlas_resol, Texture2D p_Splatting_HTMap_Texture, SamplerState p_Splatting_HTMap_Texture_Sampler )
{
    float2 ddx = { 0.0, 0.0 };
    float2 ddy = { 0.0, 0.0 };

	// contracter legerement les coords patch pour eviter les effets de bords de couleurs
    float4 patch_coords = (0.98 * (p_textcoords - 0.5)) + 0.5;

	//float interval = 1.0 / 16;
    float interval = 1.0 / p_splatting_atlas_resol;

    float limit_up = 1.0 - interval;

    float t, h;

    t = clamp(p_temperature, 0.0, 0.999);
    h = clamp(p_humidity, 0.0, 0.999);
	   			
    float2 temperature_sampling = compute_sampling_params(clamp(t, 0.0, 1.0 - (4.0 * interval)), interval);
    float2 humidity_sampling = compute_sampling_params(clamp(h, 0.0, 1.0 - (4.0 * interval)), interval);


    float4 res_color = 0.0;


    bool delta_u = false;
    bool delta_v = false;
	
    if (t < limit_up)
    {
        delta_u = true;
    }
    else
    {
        delta_u = false;
    }

    if (h < limit_up)
    {
        delta_v = true;
    }
    else
    {
        delta_v = false;
    }

    if (delta_u && delta_v)
    {
        float4 coords_1 = patch_coords * interval;
        coords_1.x += temperature_sampling.y;
        coords_1.y += humidity_sampling.y;

        float4 coords_2 = patch_coords * interval;
        coords_2.x += temperature_sampling.y + interval;
        coords_2.y += humidity_sampling.y;

        float4 coords_3 = patch_coords * interval;
        coords_3.x += temperature_sampling.y + interval;
        coords_3.y += humidity_sampling.y + interval;

        float4 coords_4 = patch_coords * interval;
        coords_4.x += temperature_sampling.y;
        coords_4.y += humidity_sampling.y + interval;



        // NB: use SampleGrad() rather then Sample() avoid strange artifacts on textures splatting limits in HLSL 4.0
        float4 color_1 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_1.xy, ddx, ddy);
        float4 color_2 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_2.xy, ddx, ddy);
        float4 color_3 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_3.xy, ddx, ddy);
        float4 color_4 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_4.xy, ddx, ddy);

		// calcul des poids splatting : calcul d'un "vecteur des distances"

        //float4 distances;

        float2 center_point;
        center_point.x = temperature_sampling.x;
        center_point.y = humidity_sampling.x;

        float2 point_1;
        float2 point_2;
        float2 point_3;
        float2 point_4;

        point_1.x = 0.0;
        point_1.y = 0.0;

        point_2.x = 1.0;
        point_2.y = 0.0;

        point_3.x = 1.0;
        point_3.y = 1.0;

        point_4.x = 0.0;
        point_4.y = 1.0;

        res_color = lerp(lerp(color_1, color_4, center_point.y), lerp(color_2, color_3, center_point.y), center_point.x);
    }
    else if (delta_u && !delta_v)
    {
        float4 coords_1 = patch_coords * interval;
        coords_1.x += temperature_sampling.y;
        coords_1.y += humidity_sampling.y;

        float4 coords_2 = patch_coords * interval;
        coords_2.x += temperature_sampling.y + interval;
        coords_2.y += humidity_sampling.y;

        float4 color_1 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_1.xy, ddx, ddy);
        float4 color_2 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_2.xy, ddx, ddy);
		
        res_color = ((1.0 - temperature_sampling.x) * color_1) + (temperature_sampling.x * color_2);
    }
    else if (!delta_u && delta_v)
    {
        float4 coords_1 = patch_coords * interval;
        coords_1.x += temperature_sampling.y;
        coords_1.y += humidity_sampling.y;

        float4 coords_4 = patch_coords * interval;
        coords_4.x += temperature_sampling.y;
        coords_4.y += humidity_sampling.y + interval;

        float4 color_1 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_1.xy, ddx, ddy);
        float4 color_4 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_4.xy, ddx, ddy);
        res_color = ((1.0 - humidity_sampling.x) * color_1) + (humidity_sampling.x * color_4);
    }
    else
    {
        float4 coords_1 = patch_coords * interval;
        coords_1.x += temperature_sampling.y;
        coords_1.y += humidity_sampling.y;

        float4 color_1 = p_Splatting_HTMap_Texture.SampleGrad(p_Splatting_HTMap_Texture_Sampler, coords_1.xy, ddx, ddy);
		
        res_color = color_1;
    }

    return res_color;
}

float specular_light( float3 p_half_vector, float3 p_normale, float p_spec_power)
{
    return pow(saturate(dot(normalize(p_normale), p_half_vector)), p_spec_power);
}

float ocean_specular_from_space(float p_specular_val, float p_rel_alt, float3 p_ldir, float3 p_half_vector)
{    
    float spec_attenuation = 0.6;

    float alt_spec_attenuation = saturate(p_rel_alt - 1.0);

    return (p_specular_val * spec_attenuation * alt_spec_attenuation);
}

float4 reflected_vertex_pos(float4 p_vertex, float4 p_reflectorPos, float4 p_reflectorNormale, float4x4 p_matWorld, float4x4 p_matView, float4x4 p_matProj)
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
