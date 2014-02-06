/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "inertbody.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;

InertBody::InertBody( void ) : Body()
{
}

InertBody::InertBody( DrawSpace::Interface::Drawable* p_drawable ) : Body( p_drawable )
{
}

InertBody::~InertBody( void )
{
}

void InertBody::SetParameters( const Parameters& p_parameters )
{
    m_parameters = p_parameters;
}

void InertBody::GetParameters( Parameters& p_parameters )
{
    p_parameters = m_parameters;
}

void InertBody::SetInitialPos( const DrawSpace::Utils::Vector& p_pos )
{

}