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

#include "planetclimatebinder.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


PlanetClimateBinder::PlanetClimateBinder( dsreal p_plains_amplitude, dsreal p_mountains_amplitude, dsreal p_vertical_offset, dsreal p_mountains_offset ) : 
MultiFractalBinder( p_plains_amplitude, p_mountains_amplitude, p_vertical_offset, p_mountains_offset )
{
}

void PlanetClimateBinder::Bind( void )
{
	// planete temperee

	Vector thparams(40.0, 20.0, TEMP_DEC_PER_KM, BEACH_LIMIT);
	Vector thparams2(0.48, 0.87, 0.45, 0.75);

	// planete chaude et peu humide (aride) : desertique
	//Vector thparams( 0.0, 10.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
	//Vector thparams2( 0.0, 0.0, 0.78, 0.78 );

	// planete chaude et tres humide : monde tropical
	//Vector thparams( 2500.0, 1800.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
	//Vector thparams2( 0.05, 0.1, 0.45, 0.48 );


	// monde glacé et plutot sec
	//Vector thparams( 50.0, 80.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
	//Vector thparams2( 1.4, 1.5, 0.28, 0.99 );

	// monde froid et plutot humide
	//Vector thparams( 1400.0, 1900.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
	//Vector thparams2( 0.92, 1.5, 0.37, 0.99 );


	m_renderer->SetFxShaderParams( 0, 32, thparams );
	m_renderer->SetFxShaderParams( 0, 33, thparams2 );

	MultiFractalBinder::Bind();
}

void PlanetClimateBinder::Unbind( void )
{
	MultiFractalBinder::Unbind();
}
