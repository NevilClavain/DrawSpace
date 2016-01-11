
double Noise_Lattice( int ix, double fx, int iy, double fy, int iz, double fz, sampler2D TBuffer, sampler2D TMap )
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
		if( indexTemp > 255 )
		{
			indexTemp = indexTemp - 256;
		}
		else if( indexTemp < 0 )
		{
			indexTemp = 256 + indexTemp;
		}
		maptextcoord[0] = ( index256 * indexTemp ) + midinterval256;
		
		maptextcoord[1] = 0.5;
		float4 mapval = tex2Dlod( TMap, maptextcoord );		
		nIndex = mapval.x;
	}
	
	buffertextcoord[0] = ( index256 * nIndex ) + midinterval256;
	double fValue = 0.0;
	for( i = 0; i < 3; i++ )
	{ 		
		buffertextcoord[1] = ( index3 * i ) + midinterval3;
		double buffval = tex2Dlod( TBuffer, buffertextcoord );
		fValue += ( buffval * f[i] );
	}
	return fValue;
}

double Noise_Noise( double3 f, sampler2D TBuffer, sampler2D TMap )
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

	
	fValue = lerp(lerp(lerp(Noise_Lattice(n[0], r[0], n[1], r[1], n[2], r[2], TBuffer, TMap ),
							Noise_Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2], TBuffer, TMap ),
							w[0]),
						lerp(Noise_Lattice(n[0], r[0], n[1]+1, r[1]-1.0, n[2], r[2], TBuffer, TMap ),
							Noise_Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1.0, n[2], r[2], TBuffer, TMap ),
							w[0]),
						w[1]),
					lerp(lerp(Noise_Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1.0, TBuffer, TMap ),
							Noise_Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1.0, TBuffer, TMap ),
							w[0]),
						lerp(Noise_Lattice(n[0], r[0], n[1]+1, r[1]-1.0, n[2]+1, r[2]-1.0, TBuffer, TMap),
							Noise_Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1.0, n[2]+1, r[2]-1.0, TBuffer, TMap),
							w[0]),
						w[1]),
					w[2]);
					
	
	return clamp( -0.9999, 0.9999, fValue );	
}


double Fractal_fBm( double3 f, int nbOctaves, double lacunarity, double roughness, float clamp_res, sampler2D TBuffer, sampler2D TMap )
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
		fValue += Noise_Noise( fTemp, TBuffer, TMap ) * pow( fexp, -roughness );
		
		fTemp *= lacunarity;
		fexp *= lacunarity;
	}

	double res;
	if( clamp_res > 0.0 )
	{
		res = clamp( -1.0, 1.0, fValue );
	}
	else
	{
		res = fValue;
	}		
	return res;
}