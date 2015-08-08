
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);
float4x4 matWorld: register(c8);
float4x4 matView: register(c12);
float4x4 matCam: register(c16);
float4x4 matProj: register(c20);


struct VS_INPUT 
{
	float4 Position : POSITION0;
	float4 Normales : NORMAL0;
	float4 TexCoord0: TEXCOORD0;
	float4 Pos:       TEXCOORD7;
	float4 Scale:     TEXCOORD8;

};

struct VS_OUTPUT 
{
	float4 Position : POSITION0;
	float4 TexCoord0: TEXCOORD0;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	float4 centerpos = 0;
	centerpos.w = 1;
	float4 vertexpos;

	vertexpos.z = 0.0;
	vertexpos.w = 1.0;

	if( Input.Normales.x == 1.0 )
	{
		vertexpos.x = -0.5;
		vertexpos.y = 0.5;
	}	
	else if( Input.Normales.x == 2.0 )
	{
		vertexpos.x = 0.5;
		vertexpos.y = 0.5;
	}	
	else if( Input.Normales.x == 3.0 )
	{
		vertexpos.x = 0.5;
		vertexpos.y = -0.5;
	}
	else
	{
		vertexpos.x = -0.5;
		vertexpos.y = -0.5;
	}

	float4x4 inv = 0;

	inv[0][0] = 1.0;
	inv[1][1] = 1.0;
	inv[2][2] = -1.0;
	inv[3][3] = 1.0;

	float4x4 final_view = mul( matView, inv );

	float4x4 localpos = 0;
	localpos[0][0] = 1.0;
	localpos[1][1] = 1.0;
	localpos[2][2] = 1.0;
	localpos[3][3] = 1.0;
	localpos[3][0] = Input.Pos.x;
	localpos[3][1] = Input.Pos.y;
	localpos[3][2] = Input.Pos.z;


	
	float4x4 world_view = mul( mul( matWorld, localpos ), final_view );




	float4 vertexpos2 = mul( centerpos, world_view );
	vertexpos2.x += vertexpos.x * Input.Scale.x;
	vertexpos2.y += vertexpos.y * Input.Scale.y;
	vertexpos2.z += vertexpos.z;	
	Output.Position = mul( vertexpos2, matProj );

	Output.TexCoord0 = Input.TexCoord0;

	return( Output );   
}
