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

#include "screenrenderingaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

ScreenRenderingAspect::ScreenRenderingAspect( void )
{
}

void ScreenRenderingAspect::AddImplementation( ScreenRenderingAspectImpl* p_impl )
{
    m_impls.push_back( p_impl );
}

void ScreenRenderingAspect::draw( void )
{
    for( auto it = m_impls.begin(); it != m_impls.end(); ++it )
    {
        (*it)->draw( this );      
    }
}