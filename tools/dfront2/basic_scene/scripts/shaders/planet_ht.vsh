
float4x4 matWorldViewProjection: register(c0);

float4   flag0:				register(c24);

	// .x -> patch orientation enum integer
	// .y -> patch sidelenght
	// .z -> planet ray
	

float4   patch_translation:	register(c25);
	/// .x, .y -> patch positionning

float4   base_uv: register(c26);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4 fbm_params: register(c27);
	// .y -> fbm input half-range

float4 fbm_params2: register(c28);
	// .x -> seed1
	// .y -> seed2

float4 fbm_params3: register(c29);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .w -> amplitude

float4 fbm_params4: register(c30);
	// .x -> seed1
	// .y -> seed2
	// .z -> roughness


float4 fbm_params5: register(c31);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .w -> amplitude

float4 fbm_params6: register(c32);
	// .x -> seed1
	// .y -> seed2
	// .z -> roughness


sampler2D TextureBuffer : register(s0);
sampler2D TextureMap : register(s1);


sampler2D TextureBuffer1 : register(s2);
sampler2D TextureMap1 : register(s3);

sampler2D TextureBuffer2 : register(s4);
sampler2D TextureMap2 : register(s5);


struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0;     
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0;
   float4 TexCoord1: TEXCOORD1;
};

#include "fbm.hlsl"



VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;
		
	float4 v_position;

	float temperature_min = -50.0;

	float temperature_max = 27.0;

	float temperature_alt_dec = 0.0064;  // INPUT 
	//pente temperature fct de l'altitude : 6.4 ° perdus tout les 1000 metres 

	float temperature_lat;
	float temperature_alt;
	float temperature_final;

		
	// sidelenght scaling

	v_position = Input.Position * flag0.y / 2.0;
	v_position = v_position + patch_translation;
		
	v_position.w = 1.0;
	
	float4 v_position2;

	if( 0.0 == flag0.x ) // front
	{
		v_position2.x = v_position.x;
		v_position2.y = v_position.y;
		v_position2.z = 1.0;   
	}
	else if( 1.0 == flag0.x ) // rear
	{
		v_position2.x = -v_position.x;
		v_position2.y = v_position.y;
		v_position2.z = -1.0;   
	}
	else if( 2.0 == flag0.x ) // left
	{
		v_position2.x = -1.0;
		v_position2.y = v_position.y;
		v_position2.z = v_position.x;   
	}
	else if( 3.0 == flag0.x ) // right
	{
		v_position2.x = 1.0;
		v_position2.y = v_position.y;
		v_position2.z = -v_position.x;   
	}
	else if( 4.0 == flag0.x ) // top
	{
		v_position2.x = v_position.x;
		v_position2.y = 1.0;
		v_position2.z = -v_position.y;   
	}
	else //if( 5.0 == flag0.x ) // bottom
	{
		v_position2.x = v_position.x;
		v_position2.y = -1.0;
		v_position2.z = v_position.y;
	}

	v_position2.w = v_position.w;

	float xtemp = v_position2.x;
	float ytemp = v_position2.y;
	float ztemp = v_position2.z;

	v_position2.x = xtemp * sqrt( 1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp / 3.0 );
	v_position2.y = ytemp * sqrt( 1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp / 3.0 );
	v_position2.z = ztemp * sqrt( 1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp / 3.0 );
      
	
	float h = sqrt( ( v_position2.x * v_position2.x ) + ( v_position2.z * v_position2.z ) );  // h compris entre 0.0 et 1.0


	float vertex_latitude = acos( h );


	float res;

	double n_vpos_x = ( v_position2.x / 2.0 ) + 0.5;
	double n_vpos_y = ( v_position2.y / 2.0 ) + 0.5;
	double n_vpos_z = ( v_position2.z / 2.0 ) + 0.5;

	double3 f2;
	f2[0] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_x );
	f2[1] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_y );
	f2[2] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_z );

	float fbm2 = Fractal_fBm( f2, 7, fbm_params3.x, fbm_params4.z, 0.0, fbm_params4.x, fbm_params4.y );
	if( fbm2 < 0.0 )
	{
		fbm2 = 0.0;
	}
	

	
	double3 f3;
	f3[0] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_x );
	f3[1] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_y );
	f3[2] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_z );

	float fbm3 = Fractal_fBm( f3, 7, fbm_params5.x, fbm_params6.z, 1.0, fbm_params6.x, fbm_params6.y );
	
	double3 f;
	f[0] = lerp( -fbm_params.y, fbm_params.y, n_vpos_x );
	f[1] = lerp( -fbm_params.y, fbm_params.y, n_vpos_y );
	f[2] = lerp( -fbm_params.y, fbm_params.y, n_vpos_z );

	float pn = SimplexPerlin3D( f, fbm_params2.x, fbm_params2.y );


	double3 f_humidity;
	f_humidity[0] = lerp( -fbm_params.y * 0.5, fbm_params.y * 0.5, n_vpos_x );
	f_humidity[1] = lerp( -fbm_params.y * 0.5, fbm_params.y * 0.5, n_vpos_y );
	f_humidity[2] = lerp( -fbm_params.y * 0.5, fbm_params.y * 0.5, n_vpos_z );

	float pn_humidity_variation = SimplexPerlin3D( f_humidity, fbm_params2.x, fbm_params2.y );

	double3 f_temperature;
	f_temperature[0] = lerp( -fbm_params.y * 0.5, fbm_params.y * 0.5, n_vpos_z );
	f_temperature[1] = lerp( -fbm_params.y * 0.5, fbm_params.y * 0.5, n_vpos_x );
	f_temperature[2] = lerp( -fbm_params.y * 0.5, fbm_params.y * 0.5, n_vpos_y );

	float pn_temperature_variation = SimplexPerlin3D( f_temperature, fbm_params2.y, fbm_params2.x );
	


	float pn2 = 9.0 * ( ( 0.5 * pn ) + 0.5 );
	float weight = exp( pn2 ) / 1000.0;
	

	double hl = clamp( fbm3, 0.0, 1.0 );
	res = lerp( fbm3 * fbm_params5.w, fbm3 * fbm_params5.w + fbm2 * fbm_params3.w, hl * weight );

	Output.TexCoord1 = 0.0;
	Output.TexCoord1.x = res;



	// calcul facteur temperature


	// calcul temperature en fct de la lattitude du vertex


	float norm_latitude = 1.0 - ( 2.0 * vertex_latitude / 3.1415927 );


	float temp_x;

	////////////////////////////////////////// INPUT


	// planete temperee

	
	float lim_polar = 0.48;
	float lim_tropical = 0.75;
	float k_polar = 0.45;
	float k_tropical = 0.55;

	float humidity_base = 600.0;
	float humidity_k = 300.0;
	

	// planete chaude et peu humide (aride) : desertique
	/*
	float lim_polar = 0.05;
	float lim_tropical = 0.1;
	float k_polar = 0.45;
	float k_tropical = 0.48;

	float humidity_base = 10.0;
	float humidity_k = 0.0;
	*/

	// planete chaude et tres humide : monde tropical

	/*
	float lim_polar = 0.05;
	float lim_tropical = 0.1;
	float k_polar = 0.45;
	float k_tropical = 0.48;

	float humidity_base = 1800.0;
	float humidity_k = 2500.0;
	*/

	// monde glacé et plutot sec
	/*
	float lim_polar = 1.4;
	float lim_tropical = 1.5;
	float k_polar = 0.45;
	float k_tropical = 0.99;

	float humidity_base = 80.0;
	float humidity_k = 50.0;
	*/

	// monde froid et plutot humide
	/*
	float lim_polar = 0.92;
	float lim_tropical = 1.5;
	float k_polar = 0.37;
	float k_tropical = 0.99;

	float humidity_base = 1900.0;
	float humidity_k = 1400.0;
	*/

	//////////////////////////////////////////

	if( norm_latitude < lim_polar )
	{
		// zone polaire

		temp_x = k_polar * norm_latitude /  lim_polar;
	}
	else if( norm_latitude < lim_tropical )
	{
		// zone temperee

		temp_x = k_polar + ( k_tropical - k_polar ) * ( norm_latitude - lim_polar ) /  ( lim_tropical - lim_polar );
	}
	else
	{
		// zone tropicale

		temp_x = k_tropical + ( 1.0 - k_tropical ) * ( norm_latitude - lim_tropical ) / ( 1.0 - lim_tropical );
	}



	temperature_lat = ( ( temperature_max - temperature_min ) * temp_x ) + temperature_min;
	
	// calcul de la baisse de temperature en fct de l'altitude
	float temp_dec = temperature_alt_dec * clamp( 2.0 * pn_temperature_variation, 0.6, 2.0 ) * res;

	temperature_alt = temperature_lat - temp_dec;
	temperature_final = clamp( temperature_alt, temperature_min, temperature_max );
	float color_temp = ( temperature_final - temperature_min ) / ( temperature_max - temperature_min );

	// calcul facteur humidite

	float color_humidity;
	
	float humidity_alt_max;
	
	humidity_alt_max = humidity_k * pn_humidity_variation + humidity_base;

	float res2 = res;

	if( res2 > humidity_alt_max )
	{
		res2 = humidity_alt_max;
	}
	else if( res2 < 0.0 )
	{
		res2 = 0.0;
	}
	
	color_humidity = 1.0 - ( res2 / humidity_alt_max );

	Output.TexCoord1.y = color_temp;
	Output.TexCoord1.z = color_humidity;

	Output.Position = mul( Input.Position, matWorldViewProjection );
	
	Output.TexCoord0 = 0.0;
				  
	return( Output );   
}
