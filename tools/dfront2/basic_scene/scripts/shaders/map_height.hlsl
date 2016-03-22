


float ComputeVertexHeight( float4 p_vpos, float4 p_uv )
{
	float res = 0.0;

	double n_vpos_x = ( p_vpos.x / 2.0 ) + 0.5;
	double n_vpos_y = ( p_vpos.y / 2.0 ) + 0.5;
	double n_vpos_z = ( p_vpos.z / 2.0 ) + 0.5;


	double3 f2;
	f2[0] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_x );
	f2[1] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_y );
	f2[2] = lerp( -fbm_params3.y, fbm_params3.y, n_vpos_z );

	float fbm2 = Fractal_fBm_classic_perlin( f2, 7, fbm_params3.x, fbm_params4.z, 0.0 );
	if( fbm2 < 0.0 )
	{
		fbm2 = 0.0;
	}


	double3 f3;
	f3[0] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_x );
	f3[1] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_y );
	f3[2] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_z );

	float fbm3 = Fractal_fBm_wombat_perlin( f3, 7, fbm_params5.x, fbm_params6.z, 1.0, fbm_params6.x, fbm_params6.y );

    
    /////////////////////////////////////////////////////////////////////////////// 
    float4 uv_mod = 0.0;

    uv_mod.x = p_uv.x + ( fbm3 * 0.025 );
    uv_mod.y = p_uv.y + ( -fbm3 * 0.025 );
    
    float4 color = tex2Dlod( TexturePlanetMap, uv_mod );
    ////////////////////////////////////////////////////////////////////////////////////////

    
    float plain = fbm_params5.w * color.y;
    float mountains = fbm2 * fbm_params3.w + 1000.0;

    
	float pn = color.z;

	float pn2 = 9.0 * pn;
	float weight = exp( pn2 ) / 1000.0;
	

	double hl = color.y;
	res = lerp( plain, plain + mountains, hl * weight );
	res += fbm_params.z;
    

    return res;
}


