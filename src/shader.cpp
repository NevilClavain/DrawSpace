/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "shader.h"
#include "file.h"
#include "memalloc.h"
#include "exceptions.h"
#include "misc_utils.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


Shader::Shader( void ) :
m_data( NULL ), 
m_compiled( false ), 
m_datasize( -1 )
{
    // properties array creation
    m_properties["filepath"].AddPropValue<dsstring>( m_path );
    m_properties["assetname"].AddPropValue<dsstring>( m_assetname );
    m_properties["compiled"].AddPropValue<bool>( m_compiled );
}

Shader::Shader( const dsstring& p_path, bool p_compiled ) : 
m_path( p_path ), 
m_data( NULL ), 
m_compiled( p_compiled ), 
m_datasize( -1 )
{
    // properties array creation
    m_properties["filepath"].AddPropValue<dsstring>( m_path );
    m_properties["assetname"].AddPropValue<dsstring>( m_assetname );
    m_properties["compiled"].AddPropValue<bool>( m_compiled );
}

Shader::Shader( bool p_compiled ) : 
m_data( NULL ), 
m_compiled( p_compiled ), 
m_datasize( -1 )
{
    // properties array creation
    m_properties["filepath"].AddPropValue<dsstring>( m_path );
    m_properties["assetname"].AddPropValue<dsstring>( m_assetname );
    m_properties["compiled"].AddPropValue<bool>( m_compiled );
}

Shader::~Shader( void )
{
}

bool Shader::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
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
    else if( "compiled" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["compiled"].SetPropValue<bool>( "true" == p_words[1] ? true : false );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

bool Shader::IsCompiled( void )
{
    return m_compiled;
}

void* Shader::GetData( void )
{
    return m_data;
}

size_t Shader::GetDataSize( void )
{
    return m_datasize;
}

bool Shader::LoadFromFile( void )
{
    long size;
    void* data = Utils::File::LoadAndAllocBinaryFile( m_path, &size );
    if( !data )
    {
        return false;
    }
    m_data = data;
    m_datasize = size;
    return true;
}

void Shader::ReleaseData( void )
{
    if( m_data )
    {
        _DRAWSPACE_DELETE_N_( m_data );
        m_data = NULL;
    }
}

bool Shader::ApplyProperties( void )
{
    m_path = m_properties["filepath"].GetPropValue<dsstring>();
    if( false == LoadFromFile() )
    {
        return false;
    }
    m_assetname = m_properties["assetname"].GetPropValue<dsstring>();

    m_compiled = m_properties["compiled"].GetPropValue<bool>();

    return true;
}

void Shader::Serialize( Archive& p_archive  )
{


}

bool Shader::Unserialize( Archive& p_archive )
{
    return false;
}

void Shader::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    //p_text = "declare_asset ";
    //p_text += dsstring( SHADER_TEXT_KEYWORD );

    //p_text += "\n";

    p_text += "assetname ";
    p_text += m_properties["assetname"].GetPropValue<dsstring>();
    p_text += "\r\n";

    p_text += "filepath ";
    p_text += m_properties["filepath"].GetPropValue<dsstring>();
    p_text += "\r\n";

    p_text += "compiled ";

    if( m_properties["compiled"].GetPropValue<bool>() )
    {
        p_text += "true";
    }
    else
    {
        p_text += "false";
    }
    p_text += "\r\n";

    //p_text += "end_asset\n";
}

bool Shader::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void Shader::SetText( const dsstring& p_text )
{
    m_data = (void *)_DRAWSPACE_NEW_EXPLICIT_SIZE_( char, char[p_text.size()], p_text.size() );
    m_datasize = p_text.size();
    memcpy( m_data, (void *)p_text.c_str(), p_text.size() );
}

Asset* Shader::Instanciate( void )
{
    return _DRAWSPACE_NEW_( Shader, Shader );
}

void Shader::GetKeyword( dsstring& p_outkeyword )
{
    p_outkeyword = SHADER_TEXT_KEYWORD;
}
