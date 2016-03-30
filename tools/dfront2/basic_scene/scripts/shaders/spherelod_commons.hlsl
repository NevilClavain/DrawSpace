
float3 ProjectVectorToCube( int p_orientation, float3 p_vector )
{
    float3 res;

	if( 0 == p_orientation ) // front
	{
		res.x = p_vector.x;
		res.y = p_vector.y;
		res.z = p_vector.z;   
	}
	else if( 1 == p_orientation ) // rear
	{
		res.x = -p_vector.x;
		res.y = p_vector.y;
		res.z = -p_vector.z;   
	}
	else if( 2 == p_orientation ) // left
	{
		res.x = -p_vector.z;
		res.y = p_vector.y;
		res.z = p_vector.x;   
	}
	else if( 3 == p_orientation ) // right
	{
		res.x = p_vector.z;
		res.y = p_vector.y;
		res.z = -p_vector.x;   
	}
	else if( 4 == p_orientation ) // top
	{
		res.x = p_vector.x;
		res.y = p_vector.z;
		res.z = -p_vector.y;   
	}
	else //if( 5 == p_orientation ) // bottom
	{
		res.x = p_vector.x;
		res.y = -p_vector.z;
		res.z = p_vector.y;
	}

    return res;
}

float3 CubeToSphere( float3 p_vector )
{
    float3 res;
	float xtemp = p_vector.x;
	float ytemp = p_vector.y;
	float ztemp = p_vector.z;

	res.x = xtemp * sqrt( 1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp / 3.0 );
	res.y = ytemp * sqrt( 1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp / 3.0 );
	res.z = ztemp * sqrt( 1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp / 3.0 );

    return res;
}

// a partir des textures coords sur range [0 - 1], genere un point comme sur un patch front de vertex shader
float3 compute_front_face_point_vector( float2 p_tex_coords )
{
	float3 texel_pos = 0.0;

	// x - u -> passage de [0, 1] à [-1, 1]
	texel_pos.x = ( 2.0 * p_tex_coords.x ) - 1.0;

	// y - v -> passage de [0, 1] à [+1,-1]
	texel_pos.y = -( ( 2.0 * p_tex_coords.y ) - 1.0 );

	texel_pos.z = 1.0;

    return texel_pos;
}

// genere un vecteur de bump mapping, c a d un vecteur biaise par le relief autour du point courant
float3 compute_terrain_bump_vector( float p_point_alt, int p_texture_resol, sampler2D p_htmap_texture, float2 p_tex_coords, float p_vector_bias )
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

	float4 temp_humidity_left = tex2D( p_htmap_texture, left_coords );
	float4 temp_humidity_right = tex2D( p_htmap_texture, right_coords );
	float4 temp_humidity_up = tex2D( p_htmap_texture, up_coords );
	float4 temp_humidity_down = tex2D( p_htmap_texture, down_coords );



	float point_alt_center = p_point_alt;

	float point_alt_left = temp_humidity_left.w;
	float point_alt_right = temp_humidity_right.w;
	float point_alt_up = temp_humidity_up.w;
	float point_alt_down = temp_humidity_down.w;

	//float real_texel_size = 100.0; // controle de "l'intensité" du relief bump mapping

	float3 vec_left;
	vec_left.x = -p_vector_bias;
	vec_left.y = 0.0;
	vec_left.z = ( point_alt_left - point_alt_center );

	float3 vec_right;
	vec_right.x = p_vector_bias;
	vec_right.y = 0.0;
	vec_right.z = ( point_alt_right - point_alt_center );

	float3 vec_up;
	vec_up.x = 0.0;
	vec_up.y = -p_vector_bias;
	vec_up.z = ( point_alt_up - point_alt_center );

	float3 vec_down;
	vec_down.x = 0.0;
	vec_down.y = p_vector_bias;
	vec_down.z = ( point_alt_down - point_alt_center );

	
	float3 vec1 = normalize( cross( vec_right, vec_down ) );
	float3 vec2 = normalize( cross( vec_down, vec_left ) );
	float3 vec3 = normalize( cross( vec_left, vec_up ) );
	float3 vec4 = normalize( cross( vec_up, vec_right ) );

	avg = normalize( 0.25 * ( vec1 + vec2 + vec3 + vec4 ) );	

    return avg;
}

float2 compute_sampling_params( float p_param, float p_interval )
{
	float3 res;
	
	float t_offset = p_interval * floor( p_param / p_interval );

	float w2 = ( p_param - t_offset ) / p_interval;

	res.x = w2;
	res.y = t_offset;

	return res;
}

float4 splatting_color( float4 p_textcoords, float p_temperature, float p_humidity )
{

	// contracter legerement les coords patch pour eviter les effets de bords de couleurs
	float4 patch_coords = ( 0.98 * ( p_textcoords - 0.5 ) ) + 0.5;

	//float interval = 1.0 / 16;
	float interval = 1.0 / flags6.x;

	float limit_up = 1.0 - interval;

	float t, h;

	t = clamp( p_temperature, 0.0, 0.999 );
	h = clamp( p_humidity, 0.0, 0.999 );
	   			
	float2 temperature_sampling = compute_sampling_params( clamp( t, 0.0, 1.0 - ( 4.0 * interval ) ), interval );
	float2 humidity_sampling = compute_sampling_params( clamp( h, 0.0, 1.0 - ( 4.0 * interval ) ), interval );


	float4 res_color = 0.0;


	bool delta_u = false;
	bool delta_v = false;
	
	if( t < limit_up )
	{
		delta_u = true;		
	}
	else
	{
		delta_u = false;
	}

	if( h < limit_up )
	{
		delta_v = true;
	}
	else
	{
		delta_v = false;
	}

	if( delta_u && delta_v )
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

		float4 color_1 = tex2D( Splatting_HTMap_Texture, coords_1 );
		float4 color_2 = tex2D( Splatting_HTMap_Texture, coords_2 );
		float4 color_3 = tex2D( Splatting_HTMap_Texture, coords_3 );
		float4 color_4 = tex2D( Splatting_HTMap_Texture, coords_4 );

		// calcul des poids splatting : calcul d'un "vecteur des distances"

		float4 distances;

		float2 point;
		point.x = temperature_sampling.x;
		point.y = humidity_sampling.x;

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

		distances.x = 1.0 - clamp( sqrt( ( point_1.x - point.x ) * ( point_1.x - point.x ) + ( point_1.y - point.y ) * ( point_1.y - point.y ) ), 0.0, 1.0 );
		distances.y = 1.0 - clamp( sqrt( ( point_2.x - point.x ) * ( point_2.x - point.x ) + ( point_2.y - point.y ) * ( point_2.y - point.y ) ), 0.0, 1.0 );
		distances.z = 1.0 - clamp( sqrt( ( point_3.x - point.x ) * ( point_3.x - point.x ) + ( point_3.y - point.y ) * ( point_3.y - point.y ) ), 0.0, 1.0 );
		distances.w = 1.0 - clamp( sqrt( ( point_4.x - point.x ) * ( point_4.x - point.x ) + ( point_4.y - point.y ) * ( point_4.y - point.y ) ), 0.0, 1.0 );
		
		res_color = distances.x * color_1 + distances.y * color_2 
					+ distances.z * color_3 + distances.w * color_4;
	}
	else if( delta_u && !delta_v )
	{
		float4 coords_1 = patch_coords * interval;
		coords_1.x += temperature_sampling.y;
		coords_1.y += humidity_sampling.y;

		float4 coords_2 = patch_coords * interval;
		coords_2.x += temperature_sampling.y + interval;
		coords_2.y += humidity_sampling.y;

		float4 color_1 = tex2D( Splatting_HTMap_Texture, coords_1 );
		float4 color_2 = tex2D( Splatting_HTMap_Texture, coords_2 );
		
		res_color = ( ( 1.0 - temperature_sampling.x ) * color_1 ) + ( temperature_sampling.x * color_2 );
	}
	else if( !delta_u && delta_v )
	{
		float4 coords_1 = patch_coords * interval;
		coords_1.x += temperature_sampling.y;
		coords_1.y += humidity_sampling.y;

		float4 coords_4 = patch_coords * interval;
		coords_4.x += temperature_sampling.y;
		coords_4.y += humidity_sampling.y + interval;

		float4 color_1 = tex2D( Splatting_HTMap_Texture, coords_1 );
		float4 color_4 = tex2D( Splatting_HTMap_Texture, coords_4 );

		res_color = ( ( 1.0 - humidity_sampling.x ) * color_1 ) + ( humidity_sampling.x * color_4 );
	}
	else
	{
		float4 coords_1 = patch_coords * interval;
		coords_1.x += temperature_sampling.y;
		coords_1.y += humidity_sampling.y;

		float4 color_1 = tex2D( Splatting_HTMap_Texture, coords_1 );
		
		res_color = color_1;	
	}

	return res_color;
}


