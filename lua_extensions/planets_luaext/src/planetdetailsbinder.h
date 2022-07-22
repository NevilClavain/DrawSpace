/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#pragma once

#include "multifractalbinder.h"

class PlanetLight
{
public:

	PlanetLight(void) :
		m_enable(false)
	{
	};

	bool														m_enable;
	DrawSpace::Utils::Vector									m_color;
	DrawSpace::Utils::Vector									m_dir;
	DrawSpace::Utils::Vector									m_local_dir;

};


class PlanetDetailsBinder : public MultiFractalBinder
{
public:

    bool														m_ambient;
    DrawSpace::Utils::Vector									m_ambient_color;
    PlanetLight													m_lights[3];

protected:

	dsreal														m_innerRadius;
	dsreal														m_outerRadius;
	DrawSpace::Utils::Vector									m_waveLength;
	dsreal														m_kr;
	dsreal														m_km;
	dsreal														m_scaleDepth;

    dsreal                                                      m_fog_alt_limit;
    dsreal                                                      m_fog_density;


	dsreal														m_skyfromspace_ESun;
	dsreal														m_skyfromatmo_ESun;
	dsreal														m_groundfromspace_ESun;
	dsreal														m_groundfromatmo_ESun;

	DrawSpace::Utils::Vector									m_atmo_scattering_flags0;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags1;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags2;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags3;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags4;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags5;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags6;


	DrawSpace::Utils::Matrix									m_planet_final_transform_rots;

	bool														m_mirror_mode;

	dsreal														m_ocean_details_alt;
	dsreal														m_terrain_bump_factor;

    dsreal                                                      m_splatTransitionUpRelativeAlt;
    dsreal                                                      m_splatTransitionDownRelativeAlt;

    dsreal                                                      m_splatTextureResol;

    dsreal                                                      m_atmoThickness;

	dsreal														m_wave_texture_resol;
	dsreal														m_ocean_bump_factor;

	bool														m_oceans;

	dsreal														m_oceandetails_specularpower; // specular power for oceans surface details (low altitude)

public:

	PlanetDetailsBinder( dsreal p_planetRay, dsreal p_atmoThickness, dsreal p_plains_amplitude, dsreal p_mountains_amplitude, dsreal p_vertical_offset,
                            dsreal p_mountains_offset,
                            dsreal p_plains_seed1, dsreal p_plains_seed2, dsreal p_mix_seed1, dsreal p_mix_seed2,
                            dsreal p_terrainbump_factor, dsreal p_splatTransitionUpRelativeAlt, dsreal p_splatTransitionDownRelativeAlt,
                            int p_splatTextureResol, dsreal p_atmoKr, dsreal p_fog_alt_limit, dsreal p_fog_density, bool p_oceans, dsreal p_oceandetails_specularpower);

	dsreal inline GetOceansDetailsAlt( void ) const { return m_ocean_details_alt; };
	PlanetLight inline GetLight( int p_index ) const { return m_lights[p_index]; };
    dsreal inline GetAtmoThickness(void) const { return m_atmoThickness; };

    void EnableAtmoRender(bool p_value);
    void SetLandscapeBumpFactor( dsreal p_factor );  
    void SetAtmoKr( dsreal p_kr );
    void SetFogAltLimit( dsreal p_fogaltlimit );
    void SetFogDensity( dsreal p_fog_density );

	void SetWaveTextureResol(int p_resol);
	void SetWaterBumpFactor(dsreal p_bump_factor);

    inline void SetMirrorMode(bool p_state)
    {
        m_mirror_mode = p_state;
    }

    virtual void Bind(void);
    virtual void Unbind(void);
    void Update(const DrawSpace::Utils::Matrix& p_global_transform);

};

