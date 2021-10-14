/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "planetdetailsbinder.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

PlanetDetailsBinder::PlanetDetailsBinder( dsreal p_planetRay, dsreal p_atmoThickness, dsreal p_plains_amplitude, dsreal p_mountains_amplitude, dsreal p_vertical_offset,
                                            dsreal p_mountains_offset,
                                            dsreal p_plains_seed1, dsreal p_plains_seed2, dsreal p_mix_seed1, dsreal p_mix_seed2,
                                            dsreal p_terrainbump_factor, dsreal p_splatTransitionUpRelativeAlt, dsreal p_splatTransitionDownRelativeAlt,
                                            int p_splatTextureResol, dsreal p_atmoKr, dsreal p_fog_alt_limit, dsreal p_fog_density ) :
MultiFractalBinder( p_plains_amplitude, p_mountains_amplitude, p_vertical_offset, p_mountains_offset, p_plains_seed1, p_plains_seed2, p_mix_seed1, p_mix_seed2 ),
m_ocean_details_alt( 1.0010 ),
m_terrain_bump_factor( p_terrainbump_factor ),
m_splatTransitionUpRelativeAlt( p_splatTransitionUpRelativeAlt ),
m_splatTransitionDownRelativeAlt( p_splatTransitionDownRelativeAlt ),
m_splatTextureResol( p_splatTextureResol ),
m_kr( p_atmoKr ),
m_fog_alt_limit( p_fog_alt_limit ),
m_fog_density( p_fog_density ),
m_ocean_bump_texture_resol(512),
m_ocean_bump_factor(0.99)
{

	m_mirror_mode = false;

	m_skyfromspace_ESun = 8.7;
	m_skyfromatmo_ESun = 70.0;
	m_groundfromspace_ESun = 24.0;
	m_groundfromatmo_ESun = 12.0;

    m_atmoThickness = p_atmoThickness;

	m_innerRadius = p_planetRay;
	m_outerRadius = p_planetRay + m_atmoThickness;

	m_waveLength[0] = 0.650;
	m_waveLength[1] = 0.570;
	m_waveLength[2] = 0.475;	
	m_km = 0.0010;
	m_scaleDepth = 0.25;

	m_atmo_scattering_flags0[0] = m_outerRadius;
	m_atmo_scattering_flags0[1] = m_innerRadius;
	m_atmo_scattering_flags0[2] = m_outerRadius * m_outerRadius;
	m_atmo_scattering_flags0[3] = m_innerRadius * m_innerRadius;

	m_atmo_scattering_flags1[0] = m_scaleDepth;
	m_atmo_scattering_flags1[1] = 1.0 / m_scaleDepth;
	m_atmo_scattering_flags1[2] = 1.0 / (m_outerRadius - m_innerRadius);
	m_atmo_scattering_flags1[3] = m_atmo_scattering_flags1[2] / m_scaleDepth;

	m_atmo_scattering_flags2[0] = 1.0 / pow(m_waveLength[0], 4.0);
	m_atmo_scattering_flags2[1] = 1.0 / pow(m_waveLength[1], 4.0);
	m_atmo_scattering_flags2[2] = 1.0 / pow(m_waveLength[2], 4.0);

	m_atmo_scattering_flags3[0] = m_kr;
	m_atmo_scattering_flags3[1] = m_km;
	m_atmo_scattering_flags3[2] = 4.0 * m_kr * 3.1415927;
	m_atmo_scattering_flags3[3] = 4.0 * m_km * 3.1415927;

	m_atmo_scattering_flags4[0] = m_skyfromspace_ESun;
	m_atmo_scattering_flags4[1] = m_skyfromatmo_ESun;
	m_atmo_scattering_flags4[2] = m_groundfromspace_ESun;
	m_atmo_scattering_flags4[3] = m_groundfromatmo_ESun;

	m_atmo_scattering_flags5[0] = 3.5 * m_atmoThickness; // altitude limite de transition entre xxxfromspace_atmo_scattering et xxxfromatmo_atmo_scattering
	m_atmo_scattering_flags5[1] = m_fog_alt_limit;//FOG_ALT_LIMIT; // altitude debut d'apparition du fog "sol"
	m_atmo_scattering_flags5[2] = m_fog_density; // intensite fog "sol"
	m_atmo_scattering_flags5[3] = 0.0;

	// couleurs fog "sol"    
	m_atmo_scattering_flags6[0] = 0.45;
	m_atmo_scattering_flags6[1] = 0.63;
	m_atmo_scattering_flags6[2] = 0.78;
	m_atmo_scattering_flags6[3] = 1.0;

    
	m_lights[0].m_enable = false;
	m_lights[0].m_color[0] = 1.0;
	m_lights[0].m_color[1] = 0.99;
	m_lights[0].m_color[2] = 0.99;
	m_lights[0].m_color[3] = 1.0;
	m_lights[0].m_dir[0] = 0.0;
	m_lights[0].m_dir[1] = 1.0;
	m_lights[0].m_dir[2] = 1.0;
	m_lights[0].m_dir[3] = 1.0;

	m_lights[0].m_dir.Normalize();

	m_lights[1].m_enable = false;
	m_lights[1].m_color[0] = 1.0;
	m_lights[1].m_color[1] = 0.9;
	m_lights[1].m_color[2] = 0.9;
	m_lights[1].m_color[3] = 1.0;
	m_lights[1].m_dir[0] = -1.0;
	m_lights[1].m_dir[1] = 0.2;
	m_lights[1].m_dir[2] = 0.0;
	m_lights[1].m_dir[3] = 1.0;

	m_lights[1].m_dir.Normalize();

	m_lights[2].m_enable = false;
	m_lights[2].m_color[0] = 1.0;
	m_lights[2].m_color[1] = 0.5;
	m_lights[2].m_color[2] = 0.5;
	m_lights[2].m_color[3] = 1.0;
	m_lights[2].m_dir[0] = 0.0;
	m_lights[2].m_dir[1] = -1.0;
	m_lights[2].m_dir[2] = 0.0;
	m_lights[2].m_dir[3] = 1.0;

	m_lights[2].m_dir.Normalize();

	m_ambient = false;
	m_ambient_color[0] = 0.0;
	m_ambient_color[1] = 0.0;
	m_ambient_color[2] = 0.0;
	m_ambient_color[3] = 1.0;

}

void PlanetDetailsBinder::Bind( void )
{

	Vector flags6( m_splatTextureResol, m_splatTransitionUpRelativeAlt, m_splatTransitionDownRelativeAlt, m_ocean_details_alt );

	Vector flags_lights;
	flags_lights[0] = (m_ambient ? 1.0 : 0.0);
	flags_lights[1] = (m_lights[0].m_enable ? 1.0 : 0.0);
	flags_lights[2] = (m_lights[1].m_enable ? 1.0 : 0.0);
	flags_lights[3] = (m_lights[2].m_enable ? 1.0 : 0.0);


	m_renderer->SetFxShaderParams(0, 42, m_atmo_scattering_flags0);
	m_renderer->SetFxShaderParams(0, 43, m_atmo_scattering_flags1);
	m_renderer->SetFxShaderParams(0, 44, m_atmo_scattering_flags2);
	m_renderer->SetFxShaderParams(0, 45, m_atmo_scattering_flags3);
	m_renderer->SetFxShaderParams(0, 46, m_atmo_scattering_flags4);
	m_renderer->SetFxShaderParams(0, 47, m_atmo_scattering_flags5);
	m_renderer->SetFxShaderParams(0, 48, m_atmo_scattering_flags6);

	m_renderer->SetFxShaderParams(0, 50, flags_lights);
	m_renderer->SetFxShaderParams(0, 51, m_ambient_color);

	m_renderer->SetFxShaderParams(0, 52, m_lights[0].m_local_dir);
	m_renderer->SetFxShaderParams(0, 53, m_lights[0].m_dir);
	m_renderer->SetFxShaderParams(0, 54, m_lights[0].m_color);

	m_renderer->SetFxShaderParams(0, 55, m_lights[1].m_local_dir);
	m_renderer->SetFxShaderParams(0, 56, m_lights[1].m_dir);
	m_renderer->SetFxShaderParams(0, 57, m_lights[1].m_color);

	m_renderer->SetFxShaderParams(0, 58, m_lights[2].m_local_dir);
	m_renderer->SetFxShaderParams(0, 59, m_lights[2].m_dir);
	m_renderer->SetFxShaderParams(0, 60, m_lights[2].m_color);
	m_renderer->SetFxShaderParams(0, 62, flags6);

	Vector mirror_flag;

	mirror_flag[0] = (m_mirror_mode ? 1.0 : 0.0);
	mirror_flag[1] = m_innerRadius;

	m_renderer->SetFxShaderParams(0, 61, mirror_flag);

	m_renderer->SetFxShaderParams(1, 6, flags6);

	m_renderer->SetFxShaderParams(1, 7, flags_lights);
	m_renderer->SetFxShaderParams(1, 8, m_ambient_color);

	m_renderer->SetFxShaderParams(1, 9, m_lights[0].m_local_dir);
	m_renderer->SetFxShaderParams(1, 10, m_lights[0].m_dir);
	m_renderer->SetFxShaderParams(1, 11, m_lights[0].m_color);

	m_renderer->SetFxShaderParams(1, 12, m_lights[1].m_local_dir);
	m_renderer->SetFxShaderParams(1, 13, m_lights[1].m_dir);
	m_renderer->SetFxShaderParams(1, 14, m_lights[1].m_color);

	m_renderer->SetFxShaderParams(1, 15, m_lights[2].m_local_dir);
	m_renderer->SetFxShaderParams(1, 16, m_lights[2].m_dir);
	m_renderer->SetFxShaderParams(1, 17, m_lights[2].m_color);

	m_renderer->SetFxShaderParams(1, 18, m_atmo_scattering_flags0);
	m_renderer->SetFxShaderParams(1, 19, m_atmo_scattering_flags1);
	m_renderer->SetFxShaderParams(1, 20, m_atmo_scattering_flags2);
	m_renderer->SetFxShaderParams(1, 21, m_atmo_scattering_flags3);
	m_renderer->SetFxShaderParams(1, 22, m_atmo_scattering_flags4);
	m_renderer->SetFxShaderParams(1, 23, m_atmo_scattering_flags5);
	m_renderer->SetFxShaderParams(1, 24, m_atmo_scattering_flags6);

	m_planet_final_transform_rots.Transpose(); // faire comme dans le plugin
	m_renderer->SetFxShaderMatrix(1, 25, m_planet_final_transform_rots);

	Vector water_bump_flags(m_ocean_bump_texture_resol, m_ocean_bump_factor, 0, 0);
	m_renderer->SetFxShaderParams(1, 30, water_bump_flags);

	Vector terrain_bump_flag(m_terrain_bump_factor, 0.0, 0.0, 0.0);
	m_renderer->SetFxShaderParams(1, 31, terrain_bump_flag);

	MultiFractalBinder::Bind();
}

void PlanetDetailsBinder::Unbind( void )
{
	MultiFractalBinder::Unbind();
}

void PlanetDetailsBinder::Update( const Utils::Matrix& p_global_transform )
{
	Matrix planet_final_transform = p_global_transform;

	Vector planet_pos;
	planet_pos[0] = - planet_final_transform(3, 0);
	planet_pos[1] = - planet_final_transform(3, 1);
	planet_pos[2] = - planet_final_transform(3, 2);
	planet_pos[3] = 1.0;

	planet_pos.Normalize();

	m_lights[0].m_dir.Normalize();

	planet_final_transform.ClearTranslation();
	m_planet_final_transform_rots = planet_final_transform;

	planet_final_transform.Transpose();

	for (long i = 0; i < 3; i++)
	{
		if (m_lights[i].m_enable)
		{
			planet_final_transform.Transform( &m_lights[i].m_dir, &m_lights[i].m_local_dir );

			m_lights[i].m_local_dir.Normalize();
		}
	}
}

void PlanetDetailsBinder::SetLandscapeBumpFactor( dsreal p_factor )
{
    m_terrain_bump_factor = p_factor;
}

void PlanetDetailsBinder::EnableAtmoRender( bool p_value )
{
    m_atmo_scattering_flags5[3] = ( p_value ? 1.0 : 0.0 );
}

void PlanetDetailsBinder::SetAtmoKr( dsreal p_kr )
{
    m_kr = p_kr;
	m_atmo_scattering_flags3[0] = m_kr;
	m_atmo_scattering_flags3[2] = 4.0 * m_kr * 3.1415927;
	m_atmo_scattering_flags3[3] = 4.0 * m_kr * 3.1415927;
}

void PlanetDetailsBinder::SetFogAltLimit( dsreal p_fogaltlimit )
{
    m_fog_alt_limit = p_fogaltlimit;
    m_atmo_scattering_flags5[1] = m_fog_alt_limit;//FOG_ALT_LIMIT; // altitude debut d'apparition du fog "sol"
}

void PlanetDetailsBinder::SetFogDensity( dsreal p_fog_density )
{
    m_fog_density = p_fog_density;
    m_atmo_scattering_flags5[2] = m_fog_density; // intensite fog "sol"
}

void PlanetDetailsBinder::SetWaterBumpTextureResol(int p_resol)
{
	m_ocean_bump_texture_resol = p_resol;
}

void PlanetDetailsBinder::SetWaterBumpFactor(int p_resol)
{
	m_ocean_bump_factor = p_resol;
}
