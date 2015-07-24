
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);
float4x4 matWorld: register(c8);

struct VS_INPUT 
{
	float4 Position : POSITION0;
	float4 Normales : NORMAL0;
	float4 TexCoord0: TEXCOORD0;      
};

struct VS_OUTPUT 
{
	float4 Position : POSITION0;
	float4 Normales : NORMAL0;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	float4x4 worldNoT = matWorld;
	worldNoT[3][0] = 0.0;
	worldNoT[3][1] = 0.0;
	worldNoT[3][2] = 0.0;   

	Output.Position = mul( Input.Position, matWorldViewProjection );
	Output.Normales = mul( Input.Normales, worldNoT );

      
   return( Output );   
}
