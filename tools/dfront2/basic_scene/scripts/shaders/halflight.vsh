
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);
float4x4 matWorld: register(c8);
float4x4 matView: register(c12);
float4   ldir: register(c20);

struct VS_INPUT 
{
	float4 Position : POSITION0;		     
};

struct VS_OUTPUT 
{
	float4 Position : POSITION0;
	float4 Half     : NORMAL0;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	Output.Position = mul( Input.Position, matWorldViewProjection );

	float4 CamPos;
	float4 Pos2;

	Pos2 = mul( Input.Position, matWorld );

	CamPos.x = -matView[3][0];
	CamPos.y = -matView[3][1];
	CamPos.z = -matView[3][2];
	CamPos.w = 0.0;

	Output.Half = ( normalize( -ldir ) + normalize( CamPos - Pos2 ) );

	return( Output );   
}
