
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);

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



	float4 v_position4 = mul( v_position3, matWorldView );
	float vertex_distance = sqrt( v_position4.x * v_position4.x + v_position4.y * v_position4.y + v_position4.z * v_position4.z );

	float viewer_alt = flag0.w * flag0.z;
	float horizon_limit = sqrt( viewer_alt * viewer_alt - flag0.z * flag0.z );

	float v_alt = 0.0;

	if( vertex_distance < 1.05 * horizon_limit )
	{		
		v_alt = ComputeVertexHeight( v_position2 );

		if( v_alt >= 0.0 )
		{
			// seuls les vertex "non skirt" prennent en compte l'altitude calculee du vertex;
			// les vertex "skirt" ont toujours une altitude de zero

			if( Input.TexCoord0.z == 0.0 )
			{
				v_position3 *= ( 1.0 + ( v_alt / flag0.z ) );
			}
		}
	}


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
