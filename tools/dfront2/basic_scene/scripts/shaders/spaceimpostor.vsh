
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

float4x4 InverseMatrix( float4x4 p_mat )
{
	float4x4 trans = 0;

	trans[0][0] = 1.0;
	trans[1][1] = 1.0;
	trans[2][2] = 1.0;
	trans[3][3] = 1.0;

	trans[3][0] = -p_mat[3][0];
	trans[3][1] = -p_mat[3][1];
	trans[3][2] = -p_mat[3][2];

	p_mat[3][0] = 0.0;
	p_mat[3][1] = 0.0;
	p_mat[3][2] = 0.0;

	float4x4 transp = transpose( p_mat );
	float4x4 res = mul( trans, transp );
	return res;
}

float4x4 BuildRotationMatrix( float4 p_axis, float p_angle )
{
	float4x4 rot;

	float c = cos( p_angle );
    float s = sin( p_angle );
    float omc = (1 - c);

    float4 axis = normalize( p_axis );

    float x = axis[0];
    float y = axis[1];
    float z = axis[2];	
    float xs = x * s;
    float ys = y * s;
    float zs = z * s;
    float xyomc = x * y * omc;
    float xzomc = x * z * omc;
    float yzomc = y * z * omc;

    rot[0][0] = x * x * omc + c;
    rot[0][1] = xyomc + zs;
    rot[0][2] = xzomc - ys;
    rot[0][3] = 0;
    
    rot[1][0] = xyomc - zs;
    rot[1][1] = y * y * omc + c;
    rot[1][2] = yzomc + xs;
    rot[1][3] = 0;
    
    rot[2][0] = xzomc + ys;
    rot[2][1] = yzomc - xs;
    rot[2][2] = z * z * omc + c;
    rot[2][3] = 0;
    
    rot[3][0] = 0;
    rot[3][1] = 0;
    rot[3][2] = 0;
    rot[3][3] = 1;

	return rot;
}

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


	float4 xaxis;
	xaxis.x = 1;
	xaxis.y = 0;
	xaxis.z = 0;
	xaxis.w = 0;


	float4 yaxis;
	yaxis.x = 0;
	yaxis.y = 1;
	yaxis.z = 0;
	yaxis.w = 0;

		
	float4x4 world_view = mul( matWorld, final_view );


	// passer le point ( 0, 0, 0 ) du repere cam vers le repere world	
	float4 centerpos2 = mul( centerpos, matCam );	

	// passer le point obtenu du repere world vers le repere local a l'impostor
	float4 centerpos3 = mul( centerpos2, InverseMatrix( matWorld ) );

	// il ne reste plus qu'à calculer theta et phi a partir du point obtenu (point 0 camera exprimé dans le repere local à l'impostor)

	float theta = atan2( centerpos3[0], centerpos3[2] );

	float4 theta_dir = centerpos3;
	theta_dir.y = 0;

	float phi = atan2( centerpos3[1], sqrt( theta_dir.x * theta_dir.x + theta_dir.y * theta_dir.y + theta_dir.z * theta_dir.z ) );

	float4x4 roty = BuildRotationMatrix( yaxis, theta );
	float4x4 rotx = BuildRotationMatrix( xaxis, -phi );

	float4 vertexpos2 = mul( vertexpos, mul( mul( rotx, roty ), world_view ) );
	Output.Position = mul( vertexpos2, matProj );

	Output.TexCoord0 = Input.TexCoord0;

	return( Output );   
}
