
float4x4 matWorldViewProjection: register(c0);

sampler2D TextureBuffer : register(s0);
sampler2D TextureMap : register(s1);

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

double Noise_Lattice( int ix, double fx, int iy, double fy, int iz, double fz, int iw, double fw )
{
	// ECH : texture index mid interval -> 1 /2N, N = 256 ou N = 3
	float midinterval256 = 1 / 512;
	float midinterval3 = 1 / 6;
	
	float4 maptextcoord = 0.0;
	float4 buffertextcoord = 0.0;

	int i;
	int n[4] = { ix, iy, iz, iw };
	double f[4] = { fx, fy, fz, fw };
	int nIndex = 0;
	for( i = 0; i < 3; i++ )
	{ 			
		maptextcoord[0] = clamp( 0, 255, nIndex + n[i] ) + midinterval256;
		float4 mapval = tex2Dlod( TextureMap, maptextcoord );
		nIndex = mapval.x * 255;
	}

	double fValue = 0;
	for( i = 0; i < 3; i++ )
	{ 
		buffertextcoord[0] = i + midinterval3;		
		buffertextcoord[1] = ( nIndex + n[i] ) + midinterval256;
		double buffval = tex2Dlod( TextureBuffer, buffertextcoord );
		fValue += buffval * f[i];
	}
	return fValue;
}

double Noise_Noise( double3 f )
{
	int n[3];			// Indexes to pass to lattice function
	double r[3];		// Remainders to pass to lattice function
	double w[3];		// Cubic values to pass to interpolation function

	int i;
	for( i = 0; i < 3; i++ )
	{
        n[i] = floor( f[i] );		
		r[i] = f[i] - n[i];
		double temp = r[i];
        w[i] = temp * temp * ( 3 - ( 2 * temp ) );		
	}

	double fValue;

	fValue = lerp(lerp(lerp(Noise_Lattice(n[0], r[0], n[1], r[1], n[2], r[2], 0, 0.0),
							Noise_Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2], 0, 0.0),
							w[0]),
						lerp(Noise_Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2], r[2], 0, 0.0),
							Noise_Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2], 0, 0.0),
							w[0]),
						w[1]),
					lerp(lerp(Noise_Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1, 0, 0.0),
							Noise_Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1, 0, 0.0),
							w[0]),
						lerp(Noise_Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1, 0, 0.0),
							Noise_Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1, 0, 0.0),
							w[0]),
						w[1]),
					w[2]);

	return fValue;
}


VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	Output.Position = mul( Input.Position, matWorldViewProjection );
	Output.TexCoord0 = Input.TexCoord0;
	 
	//tex2Dlod( Texture0, textcoord );


	
	/*  
	float4 textcoord = 0.0;
	float4 col;
		
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

	//col = tex2Dlod( Texture1, textcoord );
	//Output.TexCoord1 = col;
      
	Output.TexCoord1 = 0.0;
	return( Output );   
}
