
float ComputeVertexHeight( float4 p_vpos )
{
	float res;

	double n_vpos_x = ( p_vpos.x / 2.0 ) + 0.5;
	double n_vpos_y = ( p_vpos.y / 2.0 ) + 0.5;
	double n_vpos_z = ( p_vpos.z / 2.0 ) + 0.5;

	double3 f2;
	f2[0] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_x );
	f2[1] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_y );
	f2[2] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_z );

	//float fbm2 = Fractal_fBm( f2, 7, fbm_params3.x, fbm_params4.z, 0.0, fbm_params4.x, fbm_params4.y );
    float fbm2 = Fractal_fBm2( f2, 7, fbm_params3.x, fbm_params4.z, 0.0 );
	if( fbm2 < 0.0 )
	{
		fbm2 = 0.0;
	}
	

	
	double3 f3;
	f3[0] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_x );
	f3[1] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_y );
	f3[2] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_z );

	float fbm3 = Fractal_fBm( f3, 7, fbm_params5.x, fbm_params6.z, 1.0, fbm_params6.x, fbm_params6.y );
	
	double3 f;
	f[0] = lerp( -fbm_params.y, fbm_params.y, n_vpos_x );
	f[1] = lerp( -fbm_params.y, fbm_params.y, n_vpos_y );
	f[2] = lerp( -fbm_params.y, fbm_params.y, n_vpos_z );

	float pn = SimplexPerlin3D( f, fbm_params2.x, fbm_params2.y );

	float pn2 = 9.0 * ( ( 0.5 * pn ) + 0.5 );
	float weight = exp( pn2 ) / 1000.0;
	

	double hl = clamp( fbm3, 0.0, 1.0 );
	res = lerp( fbm3 * fbm_params5.w, fbm3 * fbm_params5.w + fbm2 * fbm_params3.w, hl * weight );
    

	res += fbm_params.z;

    return res;
}


