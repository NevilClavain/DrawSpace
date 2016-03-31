
float ComputeVertexHeight( float4 p_vpos, float p_plains_amplitude, float p_mountains_amplitude, float p_offset, 
                            float p_plains_seed1, float p_plains_seed2, float p_mix_seed1, float p_mix_seed2 )
{
    float fbm_mountains_hl = 16.0;
    float fbm_plains_hl    = 0.8;
    float fbm_mix_hl       = 20.0;

	float res;

	float n_vpos_x = ( p_vpos.x / 2.0 ) + 0.5;
	float n_vpos_y = ( p_vpos.y / 2.0 ) + 0.5;
	float n_vpos_z = ( p_vpos.z / 2.0 ) + 0.5;

	float3 f2;
	f2[0] = lerp( -fbm_mountains_hl, fbm_mountains_hl, n_vpos_x );
	f2[1] = lerp( -fbm_mountains_hl, fbm_mountains_hl, n_vpos_y );
	f2[2] = lerp( -fbm_mountains_hl, fbm_mountains_hl, n_vpos_z );

    float fbm_mountains = Fractal_fBm_classic_perlin( f2, 7, 2.0, 0.35, 0.0 );
	if( fbm_mountains < 0.0 )
	{
		fbm_mountains = 0.0;
	}
		
	float3 f3;
	f3[0] = lerp( -fbm_plains_hl, fbm_plains_hl, n_vpos_x );
	f3[1] = lerp( -fbm_plains_hl, fbm_plains_hl, n_vpos_y );
	f3[2] = lerp( -fbm_plains_hl, fbm_plains_hl, n_vpos_z );
	
    float fbm_plains = Fractal_fBm_wombat_perlin( f3, 7, 2.6, 0.5, 1.0, p_plains_seed1, p_plains_seed2 );
	
	float3 f;
	f[0] = lerp( -fbm_mix_hl, fbm_mix_hl, n_vpos_x );
	f[1] = lerp( -fbm_mix_hl, fbm_mix_hl, n_vpos_y );
	f[2] = lerp( -fbm_mix_hl, fbm_mix_hl, n_vpos_z );
	
    float pn = SimplexPerlin3D( f, p_mix_seed1, p_mix_seed2 );

	float pn2 = 9.0 * ( ( 0.5 * pn ) + 0.5 );
	float weight = exp( pn2 ) / 1000.0;
	
	float hl = clamp( fbm_plains, 0.0, 1.0 );

    float plains_altitude = fbm_plains * p_plains_amplitude;
    float mountains_altitude = fbm_mountains * p_mountains_amplitude;

    res = lerp( plains_altitude, plains_altitude + mountains_altitude, hl * weight );
   
    res += p_offset;

    return res;
}


