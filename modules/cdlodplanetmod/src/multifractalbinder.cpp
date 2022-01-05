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

#include "multifractalbinder.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

MultiFractalBinder::MultiFractalBinder( dsreal p_plains_amplitude, dsreal p_mountains_amplitude, dsreal p_vertical_offset, dsreal p_mountains_offset,
                                            dsreal p_plains_seed1, dsreal p_plains_seed2, dsreal p_mix_seed1, dsreal p_mix_seed2 ) :
m_plains_amplitude( p_plains_amplitude ),
m_mountains_amplitude( p_mountains_amplitude ),
m_vertical_offset( p_vertical_offset ),
m_mountains_offset( p_mountains_offset ),
m_plains_seed1( p_plains_seed1 ),
m_plains_seed2( p_plains_seed2 ),
m_mix_seed1( p_mix_seed1 ),
m_mix_seed2( p_mix_seed2 )
{
}

void MultiFractalBinder::Bind( void )
{
	Vector landscape_control;
	Vector seeds;

	landscape_control[0] = m_plains_amplitude;
	landscape_control[1] = m_mountains_amplitude;
	landscape_control[2] = m_vertical_offset;
	landscape_control[3] = m_mountains_offset;

	seeds[0] = m_plains_seed1;
	seeds[1] = m_plains_seed2;
	seeds[2] = m_mix_seed1;
	seeds[3] = m_mix_seed2;

	m_renderer->SetFxShaderParams( 0, 40, landscape_control );
	m_renderer->SetFxShaderParams( 0, 41, seeds );
}

void MultiFractalBinder::Unbind( void )
{
}


