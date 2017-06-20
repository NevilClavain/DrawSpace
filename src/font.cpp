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

#include "font.h"
#include "fontimport.h"
#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"
#include "pimanager.h"
#include "cbfgfont.h"
#include "exceptions.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

Font::Font( void ) : m_importer( NULL )/*, m_texture( NULL )*/
{
    // properties array creation
    m_properties["filespath"].AddPropValue<dsstring>( "texturefilepath", "" );
    m_properties["assetname"].AddPropValue<dsstring>( m_assetname );
    m_properties["filespath"].AddPropValue<dsstring>( "metricsfilepath", "" );
    m_properties["mode"].AddPropValue<dsstring>( "" );
}

Font::~Font( void )
{
}

bool Font::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "filespath" == p_words[0] )
    {
        if( p_words.size() < 3 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["filespath"].SetPropValue<dsstring>( "texturefilepath", p_words[1] );
        m_properties["filespath"].SetPropValue<dsstring>( "metricsfilepath", p_words[2] );
    }
    else if( "assetname" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["assetname"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "mode" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["mode"].SetPropValue<dsstring>( p_words[1] );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
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

bool Font::ApplyProperties( void )
{
    return 0;
}

void Font::Serialize( Utils::Archive& p_archive  )
{

}

bool Font::Unserialize( Utils::Archive& p_archive )
{
    return false;
}

void Font::DumpProperties( dsstring& p_text )
{
    //p_text = "declare_asset ";
    //p_text += dsstring( FONT_TEXT_KEYWORD );

    //p_text += "\n";

    p_text += "assetname ";
    p_text += m_properties["assetname"].GetPropValue<dsstring>();
    p_text += "\r\n";

    p_text += "filespath ";
    p_text += m_properties["filespath"].GetPropValue<dsstring>( "texturefilepath" );
    p_text += " ";
    p_text += m_properties["filespath"].GetPropValue<dsstring>( "metricsfilepath" );
    p_text += "\r\n";

    p_text += "plugin ";
    p_text += m_properties["mode"].GetPropValue<dsstring>();
    p_text += "\r\n";

    //p_text += "end_asset\n";

}

bool Font::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

Asset* Font::Instanciate( void )
{
    return _DRAWSPACE_NEW_( Font, Font );
}

void Font::GetKeyword( dsstring& p_outkeyword )
{
    p_outkeyword = FONT_TEXT_KEYWORD;
}
