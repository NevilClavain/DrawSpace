
float4x4 matWorldViewProjection: register(c0);
float4   zoomarea: register(c24);

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0;
      
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float4 TexCoord0: TEXCOORD0;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	Output.Position = mul( Input.Position, matWorldViewProjection );

	Output.TexCoord0 = 0;

	if( Input.TexCoord0.x == 0.0 && Input.TexCoord0.y == 0.0 )
	{
		Output.TexCoord0.x = 0.5 - zoomarea;
		Output.TexCoord0.y = 0.5 - zoomarea;
	}
	else if( Input.TexCoord0.x == 1.0 && Input.TexCoord0.y == 0.0 )
	{
		Output.TexCoord0.x = 0.5 + zoomarea;
		Output.TexCoord0.y = 0.5 - zoomarea;
	}
	else if( Input.TexCoord0.x == 0.0 && Input.TexCoord0.y == 1.0 )
	{
		Output.TexCoord0.x = 0.5 - zoomarea;
		Output.TexCoord0.y = 0.5 + zoomarea;
	}
	else if( Input.TexCoord0.x == 1.0 && Input.TexCoord0.y == 1.0 )
	{
		Output.TexCoord0.x = 0.5 + zoomarea;
		Output.TexCoord0.y = 0.5 + zoomarea;
	}
      
	return( Output );   
}
