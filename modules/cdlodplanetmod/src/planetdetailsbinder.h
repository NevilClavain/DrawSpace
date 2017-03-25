/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _PLANETDETAILSBINDER_H_
#define _PLANETDETAILSBINDER_H_

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
protected:

	dsreal														m_innerRadius;
	dsreal														m_outerRadius;
	DrawSpace::Utils::Vector									m_waveLength;
	dsreal														m_kr;
	dsreal														m_km;
	dsreal														m_scaleDepth;

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


	DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>* m_planet_node;

	bool														m_ambient;
	DrawSpace::Utils::Vector									m_ambient_color;

	PlanetLight													m_lights[3];

	DrawSpace::Utils::Matrix									m_planet_final_transform_rots;

	bool														m_mirror_mode;

	dsreal														m_ocean_details_alt;
	dsreal														m_terrain_bump_factor;

    dsreal                                                      m_splatTransitionUpRelativeAlt;
    dsreal                                                      m_splatTransitionDownRelativeAlt;

public:

	PlanetDetailsBinder( dsreal p_planetRay, dsreal p_atmoThickness, dsreal p_plains_amplitude, dsreal p_mountains_amplitude, dsreal p_vertical_offset, dsreal p_mountains_offset,
                            dsreal p_terrainbump_factor, dsreal p_splatTransitionUpRelativeAlt, dsreal p_splatTransitionDownRelativeAlt );

	virtual void Bind( void );
	virtual void Unbind( void );

	void SetPlanetNode( DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>* p_planet_node );

	void Update( void );

	void SetMirrorMode( bool p_state )
	{
		m_mirror_mode = p_state;
	}

	dsreal GetOceansDetailsAlt( void) { return m_ocean_details_alt; };

	PlanetLight GetLight( int p_index ) { return m_lights[p_index]; };
};



#endif