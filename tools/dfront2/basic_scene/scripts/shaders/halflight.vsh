
float4x4 matWorldViewProjection: register(c0);
float4x4 matWorldView: register(c4);
float4x4 matWorld: register(c8);
float4x4 matView: register(c12);
float4x4 matCam: register(c16);

float4   ldir: register(c24);

struct VS_INPUT 
{
	float4 Position : POSITION0;		     
	float4 Normales : NORMAL0;
};

struct VS_OUTPUT 
{
	float4 Position : POSITION0;	
	//float4 Normales : TEXCOORD1;
	float4 Half     : TEXCOORD2;	
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	Output.Position = mul( Input.Position, matWorldViewProjection );
	///////////////////



	float4 CamPos;
	float4 Pos2;

	Pos2 = mul( Input.Position, matWorld );
	Pos2.w = 0.0;
	
	
	CamPos.x = matCam[3][0];
	CamPos.y = matCam[3][1];
	CamPos.z = matCam[3][2];
	CamPos.w = 0.0;
	
	/*
	float4 ldir;
	ldir.x = 0.0;
	ldir.y = -1.0;
	ldir.z = 0.0;
	ldir.w = 0.0;
	*/

	float4 nldir = normalize( -ldir );
	float4 nhl = normalize( CamPos - Pos2 );
	

	Output.Half = nldir + nhl;
	///////////////////

	
	/*
	float4x4 worldNoT = matWorld;
	worldNoT[3][0] = 0.0;
	worldNoT[3][1] = 0.0;
	worldNoT[3][2] = 0.0;
	  
	Output.Normales = mul( Input.Normales, worldNoT );
	*/
	///////////////////

	return( Output );   
}
