
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

float4   base_uv2: register(c27);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4 fbm_params: register(c30);
	// .y -> fbm input half-range
	// .z -> terrain vertical offset

float4 fbm_params2: register(c31);
	// .x -> seed1
	// .y -> seed2

float4 fbm_params3: register(c32);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .z -> fbm clamp
	// .w -> amplitude

float4 fbm_params4: register(c33);
	// .x -> seed1
	// .y -> seed2
	// .z -> roughness


float4 fbm_params5: register(c34);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .z -> fbm clamp
	// .w -> amplitude

float4 fbm_params6: register(c35);
	// .x -> seed1
	// .y -> seed2
	// .z -> roughness

sampler2D TexturePlanetMap : register(s0);

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
#include "map_height.hlsl"

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	float4 v_position;

		
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


	float4 global_uv = 0.0;
	global_uv.x = lerp( base_uv2.x, base_uv2.z, Input.TexCoord0.x );
	global_uv.y = lerp( base_uv2.y, base_uv2.w, Input.TexCoord0.y );

	float res = ComputeVertexHeight( v_position2, global_uv );
      
	
	Output.TexCoord1 = 0.0;
	Output.TexCoord1.x = res;


	Output.Position = mul( Input.Position, matWorldViewProjection );
	
	Output.TexCoord0 = 0.0;

			  
	return( Output );   
}
