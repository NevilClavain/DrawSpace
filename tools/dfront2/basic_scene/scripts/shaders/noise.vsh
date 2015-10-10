
float4x4 matWorldViewProjection: register(c0);

sampler2D Texture0 : register(s0);
sampler2D Texture1 : register(s1);

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

	Output.Position = mul( Input.Position, matWorldViewProjection );
	Output.TexCoord0 = Input.TexCoord0;
	   
	float4 textcoord = 0.0;
	float4 col;
	
	/*
	textcoord[0] = 0.16;
	textcoord[1] = 0.0;
	col[0] = tex2Dlod( Texture0, textcoord );
	
	textcoord[0] = 0.4999;
	textcoord[1] = 0.0;
	col[1] = tex2Dlod( Texture0, textcoord );


	textcoord[0]= 0.83;
	textcoord[1] = 0.0;	
	col[2] = tex2Dlod( Texture0, textcoord );

	col[3] = 1.0;
	*/

	col = tex2Dlod( Texture1, textcoord );


	Output.TexCoord1 = col;
      
	return( Output );   
}
