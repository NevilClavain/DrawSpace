
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
	// .z -> fbm clamp
	// .w -> amplitude

float4 fbm_params4: register(c30);
	// .x -> seed1
	// .y -> seed2
	// .z -> roughness


float4 fbm_params5: register(c31);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .z -> fbm clamp
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
      
	// final scaling
	float4 v_position3;	
	v_position3 = v_position2 * flag0.z;	
	v_position3.w = 1.0;


	float v_alt = 0.0;
	float res;


	double3 f2;
	f2[0] = lerp( -fbm_params3.y, fbm_params3.y, ( v_position2.x / 2.0 ) + 0.5 );
	f2[1] = lerp( -fbm_params3.y, fbm_params3.y, ( v_position2.y / 2.0 ) + 0.5 );
	f2[2] = lerp( -fbm_params3.y, fbm_params3.y, ( v_position2.z / 2.0 ) + 0.5 );

	float fbm2 = Fractal_fBm( f2, 7, fbm_params3.x, fbm_params4.z, 0.0, fbm_params4.x, fbm_params4.y );
	if( fbm2 < 0.0 )
	{
		fbm2 = 0.0;
	}

	

	
	double3 f3;
	f3[0] = lerp( -fbm_params5.y, fbm_params5.y, ( v_position2.x / 2.0 ) + 0.5 );
	f3[1] = lerp( -fbm_params5.y, fbm_params5.y, ( v_position2.y / 2.0 ) + 0.5 );
	f3[2] = lerp( -fbm_params5.y, fbm_params5.y, ( v_position2.z / 2.0 ) + 0.5 );

	float fbm3 = Fractal_fBm( f3, 7, fbm_params5.x, fbm_params6.z, 1.0, fbm_params6.x, fbm_params6.y );
	
	double3 f;
	f[0] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.x / 2.0 ) + 0.5 );
	f[1] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.y / 2.0 ) + 0.5 );
	f[2] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.z / 2.0 ) + 0.5 );

	float pn = SimplexPerlin3D( f, fbm_params2.x, fbm_params2.y );

	


	float pn2 = 9.0 * ( ( 0.5 * pn ) + 0.5 );
	float weight = exp( pn2 ) / 1000.0;
	

	double hl = clamp( fbm3, 0.0, 1.0 );
	res = lerp( fbm3 * fbm_params5.w, fbm3 * fbm_params5.w + fbm2 * fbm_params3.w, hl * weight );



	v_alt = res;

	
	v_position3 *= ( 1.0 + ( v_alt / flag0.z ) );
	v_position3.w = 1.0;

	Output.Position = mul( v_position3, matWorldViewProjection );
	
	Output.TexCoord0 = 0.0;
	Output.TexCoord0.x = lerp( base_uv.x, base_uv.z, Input.TexCoord0.x );
	Output.TexCoord0.y = lerp( base_uv.y, base_uv.w, Input.TexCoord0.y );

	Output.TexCoord0.z = v_alt;

	// conserver aussi les coords textures originales du patch
	Output.TexCoord1 = 0.0;
	Output.TexCoord1.x = Input.TexCoord0.x;
	Output.TexCoord1.y = Input.TexCoord0.y;

			  
	return( Output );   
}
