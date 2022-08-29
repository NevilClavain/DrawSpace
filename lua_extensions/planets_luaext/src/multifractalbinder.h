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

#include "lod_binder.h"

class MultiFractalBinder : public LOD::Binder
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

