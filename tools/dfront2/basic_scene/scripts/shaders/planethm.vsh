
float4x4 matWorldViewProjection: register(c0);
float4   flag0:				register(c24);

	// .x -> patch orientation enum integer
	// .y -> patch sidelenght
	// .z -> planet ray
	// .w -> amplitude

float4   patch_translation:	register(c25);
	/// .x, .y -> patch positionning

float4   base_uv: register(c26);
	// .x, .y -> u1, v1
	// .z, .w -> u2, v2

float4 fbm_params: register(c27);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .z -> fbm clamp
	// .w -> reserve

float4 fbm_params2: register(c28);
	// .x -> clip mode     0 -> none -1 -> clip sup 0 -> clip inf
	// .y -> clip value
	// .z -> roughness

float4 fbm_params3: register(c29);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .z -> fbm clamp
	// .w -> reserve

float4 fbm_params4: register(c30);
	// .x -> clip mode     0 -> none -1 -> clip sup 0 -> clip inf
	// .y -> clip value
	// .z -> roughness


float4 fbm_params5: register(c31);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .z -> fbm clamp
	// .w -> reserve

float4 fbm_params6: register(c32);
	// .x -> clip mode     0 -> none -1 -> clip sup 0 -> clip inf
	// .y -> clip value
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
      
	double3 f;
	f[0] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.x / 2.0 ) + 0.5 );
	f[1] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.y / 2.0 ) + 0.5 );
	f[2] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.z / 2.0 ) + 0.5 );

	float res = Fractal_fBm( f, 7, fbm_params.x, fbm_params2.z, fbm_params.z );

	if( fbm_params2.x > 0 )
	{
		if( res < fbm_params2.y )
		{
			res = fbm_params2.y;
		}
	}
	else if( fbm_params2.x < 0 )
	{
		if( res > fbm_params2.y )
		{
			res = fbm_params2.y;
		}	
	}
	
	Output.TexCoord1 = 0.0;
	Output.TexCoord1.x = res;

	Output.Position = mul( Input.Position, matWorldViewProjection );
	
	Output.TexCoord0 = 0.0;
	Output.TexCoord0.x = lerp( base_uv.x, base_uv.z, Input.TexCoord0.x );
	Output.TexCoord0.y = lerp( base_uv.y, base_uv.w, Input.TexCoord0.y );
			  
	return( Output );   
}
