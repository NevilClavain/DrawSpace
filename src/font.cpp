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


#include "font.h"
#include "fontimport.h"
#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

Font::Font( void ) : m_importer( NULL )/*, m_texture( NULL )*/
{

}

Font::~Font( void )
{

}

void Font::SetImporter( DrawSpace::Interface::FontImport* p_importer )
{
    m_importer = p_importer;
}

bool Font::Build( const dsstring& p_texturefilepath, const dsstring& p_metricsfilepath )
{
    if( NULL == m_importer )
    {
        return false;
    }

    m_texturepath = p_texturefilepath;

    bool status = m_importer->LoadFromFile( p_metricsfilepath, this );
    if( false == status )
    {
        return false;
    }

    // image set creation
    for( std::map<unsigned char, CharMapping>::iterator it = m_charset.begin(); it != m_charset.end(); ++it )
    {
        float u1 = (*it).second.u;
        float v1 = (*it).second.v;

        float u2 = (*it).second.u + (*it).second.width;
        float v2 = (*it).second.v;

        float u3 = (*it).second.u + (*it).second.width;
        float v3 = (*it).second.v + (*it).second.height;

        float u4 = (*it).second.u;
        float v4 = (*it).second.v + (*it).second.height;

        dsreal ratio = (*it).second.width / (*it).second.height;
        dsreal height = 1.0;
        dsreal width = ratio;

        (*it).second.image = _DRAWSPACE_NEW_( Image, Image( width, height, Vector( u1, v1 ), Vector( u2, v2 ), Vector( u3, v3 ), Vector( u4, v4 ) ) );
        
        Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;        
        if( false == renderer->CreateMeshe( (*it).second.image->GetMeshe(), (*it).second.image->GetRenderMesheData() ) )
        {
            return false;
        }
        
    }
    return true;
}

void Font::AddCharMapping( unsigned char p_ch, float p_u, float p_v, float p_w, float p_h )
{
    m_charset[p_ch].u = p_u;
    m_charset[p_ch].v = p_v;
    m_charset[p_ch].width = p_w;
    m_charset[p_ch].height = p_h;
    m_charset[p_ch].image = NULL;
}

bool Font::GetCharMapping( unsigned char p_char, CharMapping& p_map )
{
    if( 0 == m_charset.count( p_char ) )
    {
        return false;
    }
    p_map = m_charset[p_char];
    return true;
}

void Font::GetTexturePath( dsstring& p_texturepath )
{
    p_texturepath = m_texturepath;
}

void Font::Serialize( Core::Factory& p_factory, Utils::Archive& p_archive  )
{

}

void Font::Unserialize( Core::Factory& p_factory, Utils::Archive& p_archive )
{

}
