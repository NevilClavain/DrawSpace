/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/
/* -*-LIC_END-*- */

//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

float3 mod289(float3 x) 
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 mod289(float4 x) 
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 permute(float4 x) 
{
     return mod289(((x*34.0)+1.0)*x);
}

float4 taylorInvSqrt(float4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float3 fade(float3 t) 
{
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

float cnoise(float3 P)
{
  float3 Pi0 = floor(P); // Integer part for indexing
  float3 Pi1 = Pi0 + 1.0; // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  float3 Pf0 = frac(P); // Fractional part for interpolation
  float3 Pf1 = Pf0 - 1.0; // Fractional part - 1.0
  float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  float4 iy = float4(Pi0.yy, Pi1.yy);
  float4 iz0 = Pi0.zzzz;
  float4 iz1 = Pi1.zzzz;

  float4 ixy = permute(permute(ix) + iy);
  float4 ixy0 = permute(ixy + iz0);
  float4 ixy1 = permute(ixy + iz1);

  float4 gx0 = ixy0 * (1.0 / 7.0);
  float4 gy0 = frac(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = frac(gx0);
  float4 gz0 = 0.5 - abs(gx0) - abs(gy0);
  float4 sz0 = step(gz0, 0.0);
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  float4 gx1 = ixy1 * (1.0 / 7.0);
  float4 gy1 = frac(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = frac(gx1);
  float4 gz1 = 0.5 - abs(gx1) - abs(gy1);
  float4 sz1 = step(gz1, 0.0);
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  float3 g000 = float3(gx0.x,gy0.x,gz0.x);
  float3 g100 = float3(gx0.y,gy0.y,gz0.y);
  float3 g010 = float3(gx0.z,gy0.z,gz0.z);
  float3 g110 = float3(gx0.w,gy0.w,gz0.w);
  float3 g001 = float3(gx1.x,gy1.x,gz1.x);
  float3 g101 = float3(gx1.y,gy1.y,gz1.y);
  float3 g011 = float3(gx1.z,gy1.z,gz1.z);
  float3 g111 = float3(gx1.w,gy1.w,gz1.w);

  float4 norm0 = taylorInvSqrt(float4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  float4 norm1 = taylorInvSqrt(float4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, float3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, float3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, float3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, float3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  float3 fade_xyz = fade(Pf0);

  float4 n_z = lerp(float4(n000, n100, n010, n110), float4(n001, n101, n011, n111), fade_xyz.z);
  float2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x); 

  return 2.2 * n_xyz;
}





//
//  Wombat
//  An efficient texture-free GLSL procedural noise library
//  Source: https://github.com/BrianSharpe/Wombat
//  Derived from: https://github.com/BrianSharpe/GPU-Noise-Lib
//
//  I'm not one for copyrights.  Use the code however you wish.
//  All I ask is that credit be given back to the blog or myself when appropriate.
//  And also to let me know if you come up with any changes, improvements, thoughts or interesting uses for this stuff. :)
//  Thanks!
//
//  Brian Sharpe
//  brisharpe CIRCLE_A yahoo DOT com
//  http://briansharpe.wordpress.com
//  https://github.com/BrianSharpe
//

//
//  Perlin Noise 3D
//  Return value range of -1.0->1.0
//

float Perlin3D( float3 P, float p_seed1, float p_seed2 )
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
    float3 SOMELARGEFLOATS = float3( p_seed1, p_seed2, 668.926525 );
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

//
//  This is a modified version of Stefan Gustavson's and Ian McEwan's work at http://github.com/ashima/webgl-noise
//  Modifications are...
//  - faster random number generation
//  - analytical final normalization
//  - space scaled can have an approx feature size of 1.0
//  - filter kernel changed to fix discontinuities at tetrahedron boundaries
//

//
//  Simplex Perlin Noise 3D
//  Return value range of -1.0->1.0

float SimplexPerlin3D( float3 P, float p_seed1, float p_seed2 )
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

    //  evaluate the kernel weights ( use (0.5-x*x)^3 instead of (0.6-x*x)^4 to fix discontinuities )
    float4 kernel_weights = v1234_x * v1234_x + v1234_y * v1234_y + v1234_z * v1234_z;
    kernel_weights = max(0.5 - kernel_weights, 0.0);
    kernel_weights = kernel_weights*kernel_weights*kernel_weights;

    //	sum with the kernel and return
    return dot( kernel_weights, grad_results ) * FINAL_NORMALIZATION;
}

float Fractal_fBm_wombat_perlin( float3 f, int nbOctaves, float lacunarity, float roughness, float clamp_res, float p_seed1, float p_seed2 )
{
	int i;
	// Initialize locals
	float fValue = 0.0;
	float3 fTemp;

	fTemp = f;

	float prev = 1.0;
	float fexp = 1.0;

	// Inner loop of spectral construction, where the fractal is built
	for( i = 0; i < nbOctaves; i++ )
	{	
        fValue += Perlin3D( fTemp, p_seed1, p_seed2 ) * pow( fexp, -roughness );
		fTemp *= lacunarity;
		fexp *= lacunarity;
	}

	float res;
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

float Fractal_fBm_classic_perlin( float3 f, int nbOctaves, float lacunarity, float roughness, float clamp_res )
{
	int i;
	// Initialize locals
	float fValue = 0.0;
	float3 fTemp;

	fTemp = f;

	float fexp = 1.0;

	// Inner loop of spectral construction, where the fractal is built
	for( i = 0; i < nbOctaves; i++ )
	{	
        fValue += cnoise( fTemp );
		fTemp *= lacunarity;
		fexp *= lacunarity;
	}

	float res;
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



float iqTurbulence(float3 p, int octaves,
                   float lacunarity, float gain)
{
    float sum = 0.5;
    float freq = 1.0, amp = 1.0;
    float2 dsum = float2(0, 0);
    for (int i = 0; i < octaves; i++)
    {
        float3 n = cnoise(p * freq);
        dsum += n.yz;
        sum += amp * n.x / (1 + dot(dsum, dsum));
        freq *= lacunarity;
        amp *= gain;
    }
    return sum;
}
