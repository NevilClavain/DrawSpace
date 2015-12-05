
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

float4 fbm_params: register(c27);
	// .x -> lacunarity
	// .y -> fbm input half-range
	// .z -> fbm clamp
	// .w -> reserve

float4 fbm_params2: register(c28);
	// .x -> clip mode     0 -> none -1 -> clip sup 0 -> clip inf
	// .y -> clip value
	// .z -> roughness


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
};

double Noise_Lattice( int ix, double fx, int iy, double fy, int iz, double fz )
{
	// ECH : texture index mid interval -> 1 /2N, N = 256 ou N = 3
	double midinterval256 = 0.001953125; //1.0 / 512.0;
	double midinterval3 = 0.166666; //1.0 / 6.0;
	double index3 = 0.3333333; //1.0 / 3.0;
	double index256 = 0.00390625; //1.0 / 256.0;
	
	float4 maptextcoord = 0.0;
	float4 buffertextcoord = 0.0;

	int i;	
	int n[3] = { ix, iy, iz };
	double f[3] = { fx, fy, fz };
	
	int nIndex = 0;
	for( i = 0; i < 3; i++ )
	{
		int indexTemp = nIndex + n[i];		
		maptextcoord[0] = ( index256 * indexTemp ) + midinterval256;
		
		maptextcoord[1] = 0.5;
		float4 mapval = tex2Dlod( TextureMap, maptextcoord );		
		nIndex = mapval.x;
	}
	
	buffertextcoord[0] = ( index256 * nIndex ) + midinterval256;
	double fValue = 0.0;
	for( i = 0; i < 3; i++ )
	{ 		
		buffertextcoord[1] = ( index3 * i ) + midinterval3;
		double buffval = tex2Dlod( TextureBuffer, buffertextcoord );
		fValue += ( buffval * f[i] );
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
        w[i] = temp * temp * ( 3.0 - ( 2.0 * temp ) );		
	}

	double fValue;

	/*
	fValue = lerp(lerp(lerp(Noise_Lattice(n[0], r[0], n[1], r[1], n[2], r[2] ),
							Noise_Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2] ),
							w[0]),
						lerp(Noise_Lattice(n[0], r[0], n[1]+1, r[1]-1.0, n[2], r[2] ),
							Noise_Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1.0, n[2], r[2] ),
							w[0]),
						w[1]),
					lerp(lerp(Noise_Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1.0),
							Noise_Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1.0),
							w[0]),
						lerp(Noise_Lattice(n[0], r[0], n[1]+1, r[1]-1.0, n[2]+1, r[2]-1.0),
							Noise_Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1.0, n[2]+1, r[2]-1.0),
							w[0]),
						w[1]),
					w[2]);
					*/

	fValue = lerp(lerp(Noise_Lattice(n[0], r[0], n[1], r[1], n[2], r[2] ),
							Noise_Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2] ),
							w[0]),
						lerp(Noise_Lattice(n[0], r[0], n[1]+1, r[1]-1.0, n[2], r[2] ),
							Noise_Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1.0, n[2], r[2] ),
							w[0]),
						w[1]);
	
	return clamp( -0.9999, 0.9999, fValue );	
}


double Fractal_fBm( double3 f, int nbOctaves, double lacunarity, double roughness )
{
	int i;
	// Initialize locals
	double fValue = 0.0;
	double3 fTemp;

	fTemp = f;
	double prev = 1.0;
	double fexp = 1.0;

	// Inner loop of spectral construction, where the fractal is built
	for( i = 0; i < nbOctaves; i++ )
	{	
		float4 tc = 0.0;
		tc[0] = fTemp[0];
		tc[1] = fTemp[1];

		fValue += Noise_Noise( fTemp ) * pow( fexp, -roughness );
		
		fTemp *= lacunarity;
		fexp *= lacunarity;
	}

	double res;
	if( fbm_params.z > 0.0 )
	{
		res = clamp( -1.0, 1.0, fValue );
	}
	else
	{
		res = fValue;
	}		
	return res;
}

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;

	float4 v_position;

		
	// sidelenght scaling

	v_position = Input.Position * flag0.y / 2.0;
	v_position = v_position + patch_translation;
		
	v_position.w = 1.0;
	
	float4 v_position2;

	if( 0.0 == flag0.x ) // front
	{
		v_position2.x = v_position.x;
		v_position2.y = v_position.y;
		v_position2.z = 1.0;   
	}
	else if( 1.0 == flag0.x ) // rear
	{
		v_position2.x = -v_position.x;
		v_position2.y = v_position.y;
		v_position2.z = -1.0;   
	}
	else if( 2.0 == flag0.x ) // left
	{
		v_position2.x = -1.0;
		v_position2.y = v_position.y;
		v_position2.z = v_position.x;   
	}
	else if( 3.0 == flag0.x ) // right
	{
		v_position2.x = 1.0;
		v_position2.y = v_position.y;
		v_position2.z = -v_position.x;   
	}
	else if( 4.0 == flag0.x ) // top
	{
		v_position2.x = v_position.x;
		v_position2.y = 1.0;
		v_position2.z = -v_position.y;   
	}
	else //if( 5.0 == flag0.x ) // bottom
	{
		v_position2.x = v_position.x;
		v_position2.y = -1.0;
		v_position2.z = v_position.y;
	}

	v_position2.w = v_position.w;

	float xtemp = v_position2.x;
	float ytemp = v_position2.y;
	float ztemp = v_position2.z;

	v_position2.x = xtemp * sqrt( 1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp / 3.0 );
	v_position2.y = ytemp * sqrt( 1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp / 3.0 );
	v_position2.z = ztemp * sqrt( 1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp / 3.0 );
      
	// final scaling
	float4 v_position3;	
	v_position3 = v_position2 * flag0.z;	
	v_position3.w = 1.0;


	float v_alt = 0.0;

	double3 f;
	f[0] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.x / 2.0 ) + 0.5 );
	f[1] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.y / 2.0 ) + 0.5 );
	f[2] = lerp( -fbm_params.y, fbm_params.y, ( v_position2.z / 2.0 ) + 0.5 );

	float res = Fractal_fBm( f, 7, fbm_params.x, fbm_params2.z );
	v_alt = res * flag0.w;


	if( fbm_params2.x > 0 )
	{
		if( v_alt < fbm_params2.y )
		{
			v_alt = fbm_params2.y;
		}
	}
	else if( fbm_params2.x < 0 )
	{
		if( v_alt > fbm_params2.y )
		{
			v_alt = fbm_params2.y;
		}	
	}
	
	v_position3 *= ( 1.0 + ( v_alt / flag0.z ) );
	v_position3.w = 1.0;

	Output.Position = mul( v_position3, matWorldViewProjection );
	
	Output.TexCoord0 = 0.0;
	Output.TexCoord0.x = lerp( base_uv.x, base_uv.z, Input.TexCoord0.x );
	Output.TexCoord0.y = lerp( base_uv.y, base_uv.w, Input.TexCoord0.y );
			  
	return( Output );   
}
