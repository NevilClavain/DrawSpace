
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

#include "spherelod_fbm.h"

using namespace DrawSpace;
using namespace DrawSpace::SphericalLOD;

Fbm::Fbm( void ) :
m_InputHalfRange( 10.0 ),
m_Lacunarity( 2.0 ),
m_Roughness( 0.5 ),
m_Clamp( true ) ,
m_Amplitude( 10000.0 ),
m_Seed1( 600 ),
m_Seed2( 700 )
{
}

Fbm::~Fbm( void )
{
}

void Fbm::Initialise( void )
{
}
