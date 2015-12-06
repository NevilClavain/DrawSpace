
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


VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;
		
	// sidelenght scaling

	Output.TexCoord1 = 0.0;

	Output.Position = mul( Input.Position, matWorldViewProjection );
	
	Output.TexCoord0 = 0.0;
	Output.TexCoord0.x = lerp( base_uv.x, base_uv.z, Input.TexCoord0.x );
	Output.TexCoord0.y = lerp( base_uv.y, base_uv.w, Input.TexCoord0.y );
			  
	return( Output );   
}
