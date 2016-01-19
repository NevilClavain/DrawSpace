
float SimplexPerlin3D( float3 P, double p_seed1, double p_seed2 )
{
    //  https://github.com/BrianSharpe/Wombat/blob/master/SimplexPerlin3D.glsl

    //  simplex math constants
    float SKEWFACTOR = 1.0/3.0;
    float UNSKEWFACTOR = 1.0/6.0;
    float SIMPLEX_CORNER_POS = 0.5;
    float SIMPLEX_TETRAHADRON_HEIGHT = 0.70710678118654752440084436210485;    // sqrt( 0.5 )

    //  establish our grid cell.
    P *= SIMPLEX_TETRAHADRON_HEIGHT;    // scale space so we can have an approx feature size of 1.0
    //float3 Pi = floor( P + dot( P, float3( SKEWFACTOR) ) );
    float3 Pi;
    float3 sk = SKEWFACTOR;
    
    float prodsca = dot( P, sk );

    Pi.x = floor( P.x + prodsca );
    Pi.y = floor( P.y + prodsca );
    Pi.z = floor( P.z + prodsca );

    

    //  Find the vectors to the corners of our simplex tetrahedron
    //float3 x0 = P - Pi + dot(Pi, float3( UNSKEWFACTOR ) );


    float3 x0;
    float3 usk = UNSKEWFACTOR;

    float prodsca2 = dot( Pi, usk );

    x0 = P - Pi + prodsca2;


    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.0 - g;
    float3 Pi_1 = min( g.xyz, l.zxy );
    float3 Pi_2 = max( g.xyz, l.zxy );
    float3 x1 = x0 - Pi_1 + UNSKEWFACTOR;
    float3 x2 = x0 - Pi_2 + SKEWFACTOR;
    float3 x3 = x0 - SIMPLEX_CORNER_POS;

    //  pack them into a parallel-friendly arrangement
    float4 v1234_x = float4( x0.x, x1.x, x2.x, x3.x );
    float4 v1234_y = float4( x0.y, x1.y, x2.y, x3.y );
    float4 v1234_z = float4( x0.z, x1.z, x2.z, x3.z );

    // clamp the domain of our grid cell
    Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
    //float3 Pi_inc1 = step( Pi, float3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
    float3 Pi_inc1;
    
    Pi_inc1.x = step( Pi.x, float( 69.0 - 1.5 ) ) * ( Pi.x + 1.0 );
    Pi_inc1.y = step( Pi.y, float( 69.0 - 1.5 ) ) * ( Pi.y + 1.0 );
    Pi_inc1.z = step( Pi.z, float( 69.0 - 1.5 ) ) * ( Pi.z + 1.0 );

    //	generate the random vectors
    float4 Pt = float4( Pi.xy, Pi_inc1.xy ) + float2( 50.0, 161.0 ).xyxy;
    Pt *= Pt;
    float4 V1xy_V2xy = lerp( Pt.xyxy, Pt.zwzw, float4( Pi_1.xy, Pi_2.xy ) );
    Pt = float4( Pt.x, V1xy_V2xy.xz, Pt.z ) * float4( Pt.y, V1xy_V2xy.yw, Pt.w );
    //float3 SOMELARGEFLOATS = float3( 635.298681, 682.357502, 668.926525 );
    float3 SOMELARGEFLOATS = float3( p_seed1, p_seed2, 668.926525 );

    float3 ZINC = float3( 48.500388, 65.294118, 63.934599 );
    float3 lowz_mods = float3( 1.0 / ( SOMELARGEFLOATS.xyz + Pi.zzz * ZINC.xyz ) );
    float3 highz_mods = float3( 1.0 / ( SOMELARGEFLOATS.xyz + Pi_inc1.zzz * ZINC.xyz ) );
    Pi_1 = ( Pi_1.z < 0.5 ) ? lowz_mods : highz_mods;
    Pi_2 = ( Pi_2.z < 0.5 ) ? lowz_mods : highz_mods;
    float4 hash_0 = frac( Pt * float4( lowz_mods.x, Pi_1.x, Pi_2.x, highz_mods.x ) ) - 0.49999;
    float4 hash_1 = frac( Pt * float4( lowz_mods.y, Pi_1.y, Pi_2.y, highz_mods.y ) ) - 0.49999;
    float4 hash_2 = frac( Pt * float4( lowz_mods.z, Pi_1.z, Pi_2.z, highz_mods.z ) ) - 0.49999;

    //	evaluate gradients
    float4 grad_results = rsqrt( hash_0 * hash_0 + hash_1 * hash_1 + hash_2 * hash_2 ) * ( hash_0 * v1234_x + hash_1 * v1234_y + hash_2 * v1234_z );

    //	Normalization factor to scale the final result to a strict 1.0->-1.0 range
    //	http://briansharpe.wordpress.com/2012/01/13/simplex-noise/#comment-36
    float FINAL_NORMALIZATION = 37.837227241611314102871574478976;

    //  evaulate the kernel weights ( use (0.5-x*x)^3 instead of (0.6-x*x)^4 to fix discontinuities )
    float4 kernel_weights = v1234_x * v1234_x + v1234_y * v1234_y + v1234_z * v1234_z;
    kernel_weights = max(0.5 - kernel_weights, 0.0);
    kernel_weights = kernel_weights*kernel_weights*kernel_weights;

    //	sum with the kernel and return
    return dot( kernel_weights, grad_results ) * FINAL_NORMALIZATION;
}


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

    Pi_inc1.x = step( Pi.x, float( 69.0 - 1.5 ) ) * ( Pi.x + 1.0 );
    Pi_inc1.y = step( Pi.y, float( 69.0 - 1.5 ) ) * ( Pi.y + 1.0 );
    Pi_inc1.z = step( Pi.z, float( 69.0 - 1.5 ) ) * ( Pi.z + 1.0 );


    // calculate the hash
    float4 Pt = float4( Pi.xy, Pi_inc1.xy ) + float2( 50.0, 161.0 ).xyxy;
    Pt *= Pt;
    Pt = Pt.xzxz * Pt.yyww;
    float3 SOMELARGEFLOATS = float3( 635.298681, 682.357502, 668.926525 );
    float3 ZINC = float3( 48.500388, 65.294118, 63.934599 );
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


double Fractal_fBm( double3 f, int nbOctaves, double lacunarity, double roughness, float clamp_res, double p_seed1, double p_seed2 )
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
        fValue += SimplexPerlin3D( fTemp, p_seed1, p_seed2 ) * pow( fexp, -roughness );
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