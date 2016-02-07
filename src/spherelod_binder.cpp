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

#include "spherelod_binder.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;

Binder::Binder( void ) : 
m_renderer( NULL )
{

}

void Binder::SetRenderer( DrawSpace::Interface::Renderer* p_renderer )
{
    m_renderer = p_renderer;
}

void Binder::SetTexture( Core::Texture* p_texture, long p_stage )
{
    m_textures[p_stage] = p_texture;
}

void Binder::SetVertexTexture( Core::Texture* p_texture, long p_stage )
{
    m_vertextextures[p_stage] = p_texture;
}

void Binder::SetFx( Core::Fx* p_fx )
{
    m_fx = p_fx;
}
    
Texture* Binder::GetTexture( long p_index )
{
    return m_textures[p_index];
}

Texture* Binder::GetVertexTexture( long p_index )
{
    return m_vertextextures[p_index];
}

Fx* Binder::GetFx( void )
{
    return m_fx;
}
