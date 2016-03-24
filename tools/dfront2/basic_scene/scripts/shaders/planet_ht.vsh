
float4x4 matWorldViewProjection: register(c0);

float4   flag0:				register(c24);

	// .x -> patch orientation enum integer
	// .y -> patch sidelenght
	// .z -> planet ray
	// .w -> relative alt
	

float4   patch_translation:	register(c25);
	// .x, .y -> patch positionning

float4   base_uv: register(c26);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4 landscape_control: register(c30);
	// .x -> plains amplitude
	// .y -> mountains amplitude
	// .z -> terrain offset

float4 seeds: register(c31);
	// .x -> plains seed 1
	// .y -> plains seed 2
	// .z -> mix seed 1
	// .w -> mix seed 2


float4 thparams: register(c32);
	// .x -> humidity_k
	// .y -> humidity_base
	// .z -> temperature_alt_dec : nbre de degres centigrades perdus par km
	// .w -> beach_limit

float4 thparams2: register(c33);
	// .x -> lim_polar
	// .y -> lim_tropical
	// .z -> k_polar
	// .w -> k_tropical




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
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;
		
	float4 v_position;

	float temperature_min = -50.0;

	float temperature_max = 27.0;

	//float temperature_alt_dec = 0.0064;  // INPUT 
	float temperature_alt_dec = thparams.z / 1000.0;
	//pente temperature fct de l'altitude : x ° perdus tout les 1000 metres 

	float temperature_lat;
	float temperature_alt;
	float temperature_final;

		
	// sidelenght scaling

	v_position = Input.Position * flag0.y / 2.0;
	v_position = v_position + patch_translation;
	v_position.z = 1.0;		
	v_position.w = 1.0;
	
	float4 v_position2;
	v_position2.w = 1.0;
	v_position2.xyz = CubeToSphere( ProjectVectorToCube( flag0.x, v_position.xyz ) );
	
	float h = sqrt( ( v_position2.x * v_position2.x ) + ( v_position2.z * v_position2.z ) );  // h compris entre 0.0 et 1.0


	float vertex_latitude = acos( h );

	float res = ComputeVertexHeight( v_position2, landscape_control.x, landscape_control.y, landscape_control.z, seeds.x, seeds.y, seeds.z, seeds.w );


	Output.TexCoord1 = 0.0;
	Output.TexCoord1.x = res;


	double n_vpos_x = ( v_position2.x / 2.0 ) + 0.5;
	double n_vpos_y = ( v_position2.y / 2.0 ) + 0.5;
	double n_vpos_z = ( v_position2.z / 2.0 ) + 0.5;


	double3 f_humidity;
	float mask_input_hl = 20.0;

	f_humidity[0] = lerp( -mask_input_hl * 0.5, mask_input_hl * 0.5, n_vpos_x );
	f_humidity[1] = lerp( -mask_input_hl * 0.5, mask_input_hl * 0.5, n_vpos_y );
	f_humidity[2] = lerp( -mask_input_hl * 0.5, mask_input_hl * 0.5, n_vpos_z );

	float pn_humidity_variation = SimplexPerlin3D( f_humidity, seeds.z, seeds.w );
	
	double3 f_temperature;
	f_temperature[0] = lerp( -mask_input_hl * 0.5, mask_input_hl * 0.5, n_vpos_z );
	f_temperature[1] = lerp( -mask_input_hl * 0.5, mask_input_hl * 0.5, n_vpos_x );
	f_temperature[2] = lerp( -mask_input_hl * 0.5, mask_input_hl * 0.5, n_vpos_y );

	float pn_temperature_variation = SimplexPerlin3D( f_temperature, seeds.w, seeds.z );
	


	// calcul facteur temperature


	// calcul temperature en fct de la lattitude du vertex


	float norm_latitude = 1.0 - ( 2.0 * vertex_latitude / 3.1415927 );


	float temp_x;

	////////////////////////////////////////// INPUT

	float lim_polar = thparams2.x;
	float lim_tropical = thparams2.y;
	float k_polar = thparams2.z;
	float k_tropical = thparams2.w;

	float humidity_base = thparams.y;
	float humidity_k = thparams.x;

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
	float color_humidity_final;

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
	
	float hf = 1.0 - ( res2 / humidity_alt_max );

	color_humidity = lerp( 1.0 - hf, hf, color_temp );


	//float beach_lim = norm_latitude * 25.0 * ( lerp( 0.0, 1.0, pn_humidity_variation ) );
	float beach_lim = norm_latitude * thparams.w * ( lerp( 0.0, 1.0, pn_humidity_variation ) );

	if( res > beach_lim )
	{
		color_humidity_final = color_humidity;
	}
	
	else if( res <= beach_lim && res > 0.0 )
	{
		color_humidity_final = lerp( 0.0, color_humidity, res * 0.5 );
	}
	
	else
	{
		color_humidity_final = 0.0;
	}


	Output.TexCoord1.y = color_temp;
	Output.TexCoord1.z = color_humidity_final;

	Output.Position = mul( Input.Position, matWorldViewProjection );
	
	Output.TexCoord0 = 0.0;
				  
	return( Output );   
}
