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

#ifndef _MULTIFRACTALBINDER_H_
#define _MULTIFRACTALBINDER_H_

#include "drawspace.h"

//#define FOG_ALT_LIMIT                       10000.0
#define FLAT_CLOUDS_ALT                     3300.0
#define VOLUMETRIC_CLOUDS_DISTANCE_CLIP     60000.0
#define VOLUMETRIC_CLOUDS_ALT               2400.0
#define VOLUMETRIC_CLOUDS_MIN_SPEED_DEG_S   0.003
#define VOLUMETRIC_CLOUDS_MAX_SPEED_DEG_S   0.004
#define TEMP_DEC_PER_KM                     66.0
#define CLOUDS_PROCEDURALRULES_FILE         "planet_clouds_small_small.rules"
#define CLOUDS_HEIGHT                       750.0
#define CLOUDS_FOG_DENSITY                  0.000045
//#define FOG_DENSITY                         0.00050

class MultiFractalBinder : public DrawSpace::SphericalLOD::Binder
{
public:

	dsreal                                  m_plains_amplitude;
	dsreal                                  m_mountains_amplitude;
	dsreal                                  m_vertical_offset;
	dsreal                                  m_mountains_offset;

	dsreal                                  m_plains_seed1;
	dsreal                                  m_plains_seed2;
	dsreal                                  m_mix_seed1;
	dsreal                                  m_mix_seed2;

public:

	MultiFractalBinder( dsreal p_plains_amplitude, dsreal p_mountains_amplitude, dsreal p_vertical_offset, dsreal p_mountains_offset,
                        dsreal p_plains_seed1, dsreal p_plains_seed2, dsreal p_mix_seed1, dsreal p_mix_seed2 );

	virtual void Bind( void );
	virtual void Unbind( void );
};

#endif