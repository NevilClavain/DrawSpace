

float Perlin3D( float3 P )
{
    //  https://github.com/BrianSharpe/Wombat/blob/master/Perlin3D.glsl

    // establish our grid cell and unit position
    float3 Pi = floor(P);
    float3 Pf = P - Pi;
    float3 Pf_min1 = Pf - 1.0;

    // clamp the domain
    Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
    //float3 Pi_inc1 = step( Pi, float3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );

	float3 Pi_inc1;
	Pi_inc1.x = step( Pi.x, ( 69.0 - 1.5 ) ) * ( Pi.x + 1.0 );
	Pi_inc1.y = step( Pi.y, ( 69.0 - 1.5 ) ) * ( Pi.y + 1.0 );
	Pi_inc1.z = step( Pi.z, ( 69.0 - 1.5 ) ) * ( Pi.z + 1.0 );

    // calculate the hash
    float4 Pt = float4( Pi.xy, Pi_inc1.xy ) + float2( 50.0, 161.0 ).xyxy;
    Pt *= Pt;
    Pt = Pt.xzxz * Pt.yyww;
    //const float3 SOMELARGEFLOATS = float3( 635.298681, 682.357502, 668.926525 );
	//const float3 ZINC = float3( 48.500388, 65.294118, 63.934599 );

	//const float3 SOMELARGEFLOATS = float3( 455998.0, 13344.357502, 668.926525 );

    float3 SOMELARGEFLOATS;

    SOMELARGEFLOATS.x = 700099.0;
    SOMELARGEFLOATS.y = 10189.357502;
    SOMELARGEFLOATS.z = 668.926525;

    float3 ZINC;

    ZINC.x = 48.500388;
    ZINC.y = 65.294118;
    ZINC.z = 63.934599;


    

    float3 lowz_mod = float3( 1.0 / ( SOMELARGEFLOATS + Pi.zzz * ZINC ) );
    float3 highz_mod = float3( 1.0 / ( SOMELARGEFLOATS + Pi_inc1.zzz * ZINC ) );
    float4 hashx0 = frac( Pt * lowz_mod.xxxx );
    float4 hashx1 = frac( Pt * highz_mod.xxxx );
    float4 hashy0 = frac( Pt * lowz_mod.yyyy );
    float4 hashy1 = frac( Pt * highz_mod.yyyy );
    float4 hashz0 = frac( Pt * lowz_mod.zzzz );
    float4 hashz1 = frac( Pt * highz_mod.zzzz );

    // calculate the gradients
    float4 grad_x0 = hashx0 - 0.49999;
    float4 grad_y0 = hashy0 - 0.49999;
    float4 grad_z0 = hashz0 - 0.49999;
    float4 grad_x1 = hashx1 - 0.49999;
    float4 grad_y1 = hashy1 - 0.49999;
    float4 grad_z1 = hashz1 - 0.49999;
    float4 grad_results_0 = rsqrt( grad_x0 * grad_x0 + grad_y0 * grad_y0 + grad_z0 * grad_z0 ) * ( float2( Pf.x, Pf_min1.x ).xyxy * grad_x0 + float2( Pf.y, Pf_min1.y ).xxyy * grad_y0 + Pf.zzzz * grad_z0 );
    float4 grad_results_1 = rsqrt( grad_x1 * grad_x1 + grad_y1 * grad_y1 + grad_z1 * grad_z1 ) * ( float2( Pf.x, Pf_min1.x ).xyxy * grad_x1 + float2( Pf.y, Pf_min1.y ).xxyy * grad_y1 + Pf_min1.zzzz * grad_z1 );

    // Classic Perlin Interpolation
    float3 blend = Pf * Pf * Pf * (Pf * (Pf * 6.0 - 15.0) + 10.0);
    float4 res0 = lerp( grad_results_0, grad_results_1, blend.z );
    float4 blend2 = float4( blend.xy, float2( 1.0 - blend.xy ) );
    float final = dot( res0, blend2.zxzx * blend2.wwyy );
    return ( final * 1.1547005383792515290182975610039 );  // scale things to a strict -1.0->1.0 range  *= 1.0/sqrt(0.75)
}

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
		//fValue += Noise_Noise( fTemp, TBuffer, TMap ) * pow( fexp, -roughness );

        fValue += Perlin3D( fTemp ) * pow( fexp, -roughness );
		
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