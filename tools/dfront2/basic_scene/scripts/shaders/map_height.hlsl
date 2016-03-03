
float ComputeVertexHeight( float4 p_vpos, float4 p_uv )
{
	float res;

	double n_vpos_x = ( p_vpos.x / 2.0 ) + 0.5;
	double n_vpos_y = ( p_vpos.y / 2.0 ) + 0.5;
	double n_vpos_z = ( p_vpos.z / 2.0 ) + 0.5;

	double3 f3;
	f3[0] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_x );
	f3[1] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_y );
	f3[2] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_z );

	float fbm3 = Fractal_fBm( f3, 7, fbm_params5.x, fbm_params6.z, 1.0, fbm_params6.x, fbm_params6.y );

    //fbm3 += clamp( fbm3 + 0.75, -1.0, 1.0 );

    ///////////////////////////////////////////////////////////////////////////////
	double3 f4;
	f4[0] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_z );
	f4[1] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_x );
	f4[2] = lerp( -fbm_params5.y, fbm_params5.y, n_vpos_y );

	float fbm4 = Fractal_fBm( f4, 7, fbm_params5.x, fbm_params6.z, 1.0, fbm_params6.x, fbm_params6.y ); 
    float4 uv_mod = 0.0;

    uv_mod.x = p_uv.x + ( fbm4 * 0.02 );
    uv_mod.y = p_uv.y + ( fbm3 * 0.02 );

    float4 color = tex2Dlod( TexturePlanetMap, uv_mod );
    ////////////////////////////////////////////////////////////////////////////////////////
    res = fbm_params5.w * color;



    //res = fbm3 * fbm_params5.w;


    return res;
}


