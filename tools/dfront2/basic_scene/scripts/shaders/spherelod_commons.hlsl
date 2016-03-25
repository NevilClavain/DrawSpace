
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


