
float ComputeVertexHeight( float4 p_vpos, float4 p_uv )
{
	float res;
    float4 color =  tex2Dlod( TexturePlanetMap, p_uv );
    res = color.x * fbm_params3.w;
    return res;
}


