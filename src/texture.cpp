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

#include "texture.h"
#include "file.h"
#include "memalloc.h"
#include "exceptions.h"
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


Texture::Texture( void ) :
m_filedata( NULL ), 
m_filedatasize( -1 ), 
m_render_target( false ),
m_render_target_width( 256 ),
m_render_target_height( 256 )
{
    // properties array creation
    m_properties["filepath"].AddPropValue<dsstring>( m_path );
    m_properties["assetname"].AddPropValue<dsstring>( m_assetname );
    m_properties["rendertarget"].AddPropValue<bool>( m_render_target );
    m_properties["rendertarget_size"].AddPropValue<unsigned long>( "width", m_render_target_width );
    m_properties["rendertarget_size"].AddPropValue<unsigned long>( "height", m_render_target_height );
}

Texture::Texture( const dsstring& p_path, bool p_render_target, unsigned long p_render_target_width, unsigned long p_render_target_height ) : 
m_path( p_path ), 
m_filedata( NULL ), 
m_filedatasize( -1 ), 
m_render_target( p_render_target ),
m_render_target_width( p_render_target_width ),
m_render_target_height( p_render_target_height )
{
    // properties array creation
    m_properties["filepath"].AddPropValue<dsstring>( m_path );
    m_properties["assetname"].AddPropValue<dsstring>( m_assetname );
    m_properties["rendertarget"].AddPropValue<bool>( m_render_target );
    m_properties["rendertarget_size"].AddPropValue<unsigned long>( "width", m_render_target_width );
    m_properties["rendertarget_size"].AddPropValue<unsigned long>( "height", m_render_target_height );
}

Texture::~Texture( void )
{
}

bool Texture::LoadFromFile( void )
{
    long size;
    void* data = Utils::File::LoadAndAllocBinaryFile( m_path, &size );
    if( !data )
    {
        return false;
    }
    m_filedata = data;
    m_filedatasize = size;
    return true;
}

void Texture::ReleaseData( void )
{
    if( m_filedata )
    {
        _DRAWSPACE_DELETE_N_( m_filedata );
        m_filedata = NULL;
    }
}

void* Texture::GetData( void )
{
    return m_filedata;
}

long Texture::GetDataSize( void )
{
    return m_filedatasize;
}

void Texture::GetPath( dsstring& p_path )
{
    p_path = m_path;
}

bool Texture::IsRenderTarget( void )
{
    return m_render_target;
}

void Texture::GetRenderTargetDims( unsigned long& p_w, unsigned long& p_h )
{
    p_w = m_render_target_width;
    p_h = m_render_target_height;
}

bool Texture::ApplyProperties( void )
{
    m_path = m_properties["filepath"].GetPropValue<dsstring>();
    if( false == LoadFromFile() )
    {
        return false;
    }
    m_assetname = m_properties["assetname"].GetPropValue<dsstring>();

    m_render_target = m_properties["rendertarget"].GetPropValue<bool>();
    m_render_target_width = m_properties["rendertarget_size"].GetPropValue<unsigned long>( "width" );
    m_render_target_height = m_properties["rendertarget_size"].GetPropValue<unsigned long>( "height" );

    return true;
}

void Texture::Serialize( Archive& p_archive )
{

}

bool Texture::Unserialize( Archive& p_archive )
{
    return false;
}

void Texture::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    //p_text = "declare_asset ";
    //p_text += dsstring( TEXTURE_TEXT_KEYWORD );

    //p_text += "\n";

    p_text += "assetname ";
    p_text += m_properties["assetname"].GetPropValue<dsstring>();
    p_text += "\r\n";

    p_text += "filepath ";
    p_text += m_properties["filepath"].GetPropValue<dsstring>();
    p_text += "\r\n";

    p_text += "rendertarget ";

    if( m_properties["rendertarget"].GetPropValue<bool>() )
    {
        p_text += "true";
    }
    else
    {
        p_text += "false";
    }
    p_text += "\r\n";

    p_text += "rendertarget_size ";

    IntToString( m_properties["rendertarget_size"].GetPropValue<unsigned long>( "width" ), text_value );
    p_text += "width ";
    p_text += text_value;
    p_text += " ";

    IntToString( m_properties["rendertarget_size"].GetPropValue<unsigned long>( "height" ), text_value );
    p_text += "height ";
    p_text += text_value;

    p_text += "\r\n";

    //p_text += "end_asset\n";
}

bool Texture::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

bool Texture::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "filepath" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["filepath"].SetPropValue<dsstring>( p_words[1] );
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
    else if( "rendertarget" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["rendertarget"].SetPropValue<bool>( "true" == p_words[1] ? true : false );        
    }
    else if( "rendertarget_size" == p_words[0] )
    {
        if( p_words.size() < 5 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["rendertarget_size"].SetPropValue<unsigned long>( "width", StringToInt( p_words[2] ) );
        m_properties["rendertarget_size"].SetPropValue<unsigned long>( "height", StringToInt( p_words[4] ) );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void Texture::SetFormat( long p_width, long p_height, long p_bpp )
{
    m_width = p_width;
    m_height = p_height;

    if( -1 == p_bpp )
    {
        _DSEXCEPTION( "Unsupported pixel format for texture" );
    }

    m_bpp = p_bpp;
}

void Texture::GetFormat( long& p_width, long& p_height, long& p_bpp )
{
    p_width = m_width;
    p_height = m_height;
    p_bpp = m_bpp;
}

Asset* Texture::Instanciate( void )
{
    return _DRAWSPACE_NEW_( Texture, Texture );
}

void Texture::GetKeyword( dsstring& p_outkeyword )
{
    p_outkeyword = TEXTURE_TEXT_KEYWORD;
}