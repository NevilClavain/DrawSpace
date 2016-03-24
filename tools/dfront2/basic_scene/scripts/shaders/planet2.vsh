
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

float4   base_uv_global: register(c27);
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
   float4 TexCoord2: TEXCOORD2;
};

#include "fbm.hlsl"
#include "multifbm_height.hlsl"
#include "spherelod_commons.hlsl"

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	float4 v_position;

	// sidelenght scaling

	v_position = Input.Position * flag0.y / 2.0;
	v_position = v_position + patch_translation;
	v_position.z = 1.0;
	v_position.w = 1.0;
	
	float4 v_position2;	
	v_position2.w = 1.0;
	v_position2.xyz = CubeToSphere( ProjectVectorToCube( flag0.x, v_position.xyz ) );

	// final scaling
	float4 v_position3;	
	v_position3 = v_position2 * flag0.z;	
	v_position3.w = 1.0;

	float4 v_position4 = mul( v_position3, matWorldView );
	float vertex_distance = sqrt( v_position4.x * v_position4.x + v_position4.y * v_position4.y + v_position4.z * v_position4.z );

	float viewer_alt = flag0.w * flag0.z;
	float horizon_limit = sqrt( viewer_alt * viewer_alt - flag0.z * flag0.z );

	float v_alt = 0.0;

	if( vertex_distance < 1.015 * horizon_limit )
	{		
		v_alt = ComputeVertexHeight( v_position2, landscape_control.x, landscape_control.y, landscape_control.z, seeds.x, seeds.y, seeds.z, seeds.w );

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

	//Output.TexCoord2 = normalize( v_position3 );

	Output.TexCoord2 = 0.0;
	Output.TexCoord2.x = lerp( base_uv_global.x, base_uv_global.z, Input.TexCoord0.x );
	Output.TexCoord2.y = lerp( base_uv_global.y, base_uv_global.w, Input.TexCoord0.y );
			  
	return( Output );   
}
