/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

float ComputeVertexHeight(float4 p_vpos, float p_plains_amplitude, float p_mountains_amplitude, float p_offset, float p_mountain_offset,
                            float p_plains_seed1, float p_plains_seed2, float p_mix_seed1, float p_mix_seed2 )
{
    float iqturb_mountains_hl = 300.0;
     
    float fbm_plains_hl    = 0.8;
    float fbm_mix_hl = 16.0;


	float res;

	float n_vpos_x = ( p_vpos.x / 2.0 ) + 0.5;
	float n_vpos_y = ( p_vpos.y / 2.0 ) + 0.5;
	float n_vpos_z = ( p_vpos.z / 2.0 ) + 0.5;


    float3 f22;
    
    f22[0] = lerp(-iqturb_mountains_hl, iqturb_mountains_hl, n_vpos_x);
    f22[1] = lerp(-iqturb_mountains_hl, iqturb_mountains_hl, n_vpos_y);
    f22[2] = lerp(-iqturb_mountains_hl, iqturb_mountains_hl, n_vpos_z);

    

    float fbm_highmountains = iqTurbulence(f22, 7, 2.0, 0.46);
    if (fbm_highmountains < 0.0)
    {
        fbm_highmountains = 0.0;
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


	
    float pn = SimplexPerlin3D(f, p_mix_seed1, p_mix_seed2);

	float pn2 = 9.6 * ( ( 0.5 * pn ) + 0.5 );
	float weight = exp( pn2 ) / 1000.0;

    /////////////////////////////////////////////////////////

	
	float hl = clamp( fbm_plains - 0.48, 0.0, 1.0 );  // -0.48 -> pour diminuer la frequence d'apparition des montagnes

    float plains_altitude = fbm_plains * p_plains_amplitude;

    float highmountains_altitude = fbm_highmountains * p_mountains_amplitude + p_mountain_offset;
    if (highmountains_altitude < 0.0)
    {
        highmountains_altitude = 0.0;
    }

    res = lerp(plains_altitude, /*plains_altitude +*/ highmountains_altitude, hl * weight);

    res += p_offset;

    return res;
}

// retourne un facteur multiplicatif entre 0.0 et 1.0 et non une altitude en metres
float ComputeRiversFromTexture(sampler2D p_mapSampler, float4 p_vpos, float4 p_uv, float p_uvnoise_seed1, float p_uvnoise_seed2)
{
    float fbm_uvnoise_hl = 0.8;

    float res = 0.0;

    float n_vpos_x = (p_vpos.x / 2.0) + 0.5;
    float n_vpos_y = (p_vpos.y / 2.0) + 0.5;
    float n_vpos_z = (p_vpos.z / 2.0) + 0.5;


    float3 f3;
    f3[0] = lerp(-fbm_uvnoise_hl, fbm_uvnoise_hl, n_vpos_x);
    f3[1] = lerp(-fbm_uvnoise_hl, fbm_uvnoise_hl, n_vpos_y);
    f3[2] = lerp(-fbm_uvnoise_hl, fbm_uvnoise_hl, n_vpos_z);

    float fbm_uvnoise = Fractal_fBm_wombat_perlin(f3, 7, 2.6, 0.5, 1.0, p_uvnoise_seed1, p_uvnoise_seed2);

    
    /////////////////////////////////////////////////////////////////////////////// 
    float4 uv_mod = 0.0;

    uv_mod.x = p_uv.x + (fbm_uvnoise * 0.0025 );
    uv_mod.y = p_uv.y + (-fbm_uvnoise * 0.0025 );
    
    float4 color = tex2Dlod(p_mapSampler, uv_mod);
    ////////////////////////////////////////////////////////////////////////////////////////

    res = color.x;

    return res;
}

// renvoie une altitude comprise entre -0.0 et +700.0 
float ComputeCanyonsFromTexture(sampler2D p_mapSampler, float4 p_vpos, float4 p_uv, float p_uvnoise_seed1, float p_uvnoise_seed2)
{
    float fbm_uvnoise_hl = 0.8;

    float res = 0.0;

    float n_vpos_x = (p_vpos.x / 2.0) + 0.5;
    float n_vpos_y = (p_vpos.y / 2.0) + 0.5;
    float n_vpos_z = (p_vpos.z / 2.0) + 0.5;


    float3 f3;
    f3[0] = lerp(-fbm_uvnoise_hl, fbm_uvnoise_hl, n_vpos_x);
    f3[1] = lerp(-fbm_uvnoise_hl, fbm_uvnoise_hl, n_vpos_y);
    f3[2] = lerp(-fbm_uvnoise_hl, fbm_uvnoise_hl, n_vpos_z);

    float fbm_uvnoise = Fractal_fBm_wombat_perlin(f3, 7, 2.6, 0.5, 1.0, p_uvnoise_seed1, p_uvnoise_seed2);

    
    /////////////////////////////////////////////////////////////////////////////// 
    float4 uv_mod = 0.0;

    uv_mod.x = p_uv.x + (fbm_uvnoise * 0.0095);
    uv_mod.y = p_uv.y + (-fbm_uvnoise * 0.0095);
    
    float4 color = tex2Dlod(p_mapSampler, uv_mod);
    ////////////////////////////////////////////////////////////////////////////////////////

    res = 700.0 * color.x;

    return res;
}
