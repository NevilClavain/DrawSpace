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

#include "spherelod_landscape.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;

void Landscape::AddCollisionsShaders( DrawSpace::Core::Shader* p_shader )
{
    m_collisionsShaders.push_back( p_shader );
}

void Landscape::AddColorsTextureShaders( DrawSpace::Core::Shader* p_shader )
{
    m_colorsTextureShaders.push_back( p_shader );
}

DrawSpace::Core::Shader* Landscape::GetCollisionsShader( int p_index )
{
    return m_collisionsShaders[p_index];
}

DrawSpace::Core::Shader* Landscape::GetColorsTextureShader( int p_index )
{
    return m_colorsTextureShaders[p_index];
}

int Landscape::GetCollisionsShadersListSize( void )
{
    return m_collisionsShaders.size();
}

int Landscape::GetColorsTextureShadersListSize( void )
{
    return m_colorsTextureShaders.size();
}
