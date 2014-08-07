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

#include "spherelod_maps.h"
#include "spherelod_patch.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;

Maps::Maps( void )
{
}

Maps::~Maps( void )
{
}

bool Maps::Initialize( void )
{
    m_color_texture = _DRAWSPACE_NEW_( Texture, Texture( "mapcolor.bmp" ) );
    m_elevation_texture = _DRAWSPACE_NEW_( Texture, Texture( "map.bmp" ) );

    bool status;

    status = m_color_texture->LoadFromFile();
    status = m_elevation_texture->LoadFromFile();

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    status = renderer->CreateTexture( m_color_texture, &m_color_texture_data );
    status = renderer->CreateTexture( m_elevation_texture, &m_elevation_texture_data );

    return true;
}

dsreal Maps::GetAltFromLocalPoint( const Utils::Vector& p_point )
{
    return 0.0;
}

dsreal Maps::GetAltFromGlobalPoint( const Utils::Vector& p_point )
{
    return 0.0;
}

void* Maps::RequestTexture( TextureType p_texturetype, Patch* p_patch )
{
    switch( p_texturetype )
    {
        case ELEVATION_TEXTURE:
            return m_elevation_texture_data;            

        case COLOR_TEXTURE:
            return m_color_texture_data;
    }
    return NULL;
}