/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */


#include "jsonparser.h"
#include "memalloc.h"
#include "file.h"
#include "exceptions.h"

using namespace DrawSpace::Utils;

JSONParser::JSONParser( void ) : 
m_parse_success( false )
{
	jsmn_init( &m_parser );
}

JSONParser::~JSONParser( void )
{
}

int JSONParser::Parse( const dsstring& p_str )
{
	m_parse_success = false;
	int r = jsmn_parse( &m_parser, p_str.c_str(), p_str.size(), m_tokens, max_tokens );
	if( r > -1 )
	{
		m_nb_tokens = r;
		m_parse_success = true;
		m_text = p_str;
	}
	return r;
}

int JSONParser::GetTokenType( int p_index )
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
		// todo : exception
	}

	return m_tokens[p_index].type;
}

void JSONParser::GetTokenString( int p_index, dsstring& p_out_tokentext )
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
		// todo : exception
	}

	int start = m_tokens[p_index].start;
	int end = m_tokens[p_index].end;

	p_out_tokentext = m_text.substr( start, end - start );
}

int JSONParser::GetTokenSize( int p_index )
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
		// todo : exception
	}

	return m_tokens[p_index].size;
}

void JSONParser::ParseFromFile( const dsstring& p_filepath )
{
    long fsize;
    void* content = DrawSpace::Utils::File::LoadAndAllocBinaryFile( p_filepath, &fsize );
    if( !content )
    {
        _DSEXCEPTION( "Cannot open JSON file : " + p_filepath );
    }

    char* text = new char[fsize + 1];
    memcpy( text, content, fsize );
    text[fsize] = 0;

    int r = Parse( static_cast<char*>( text ) );

    _DRAWSPACE_DELETE_N_( content );
    delete[] text;


    if( r < 0 )
    {
        char comment[6];
        sprintf( comment, "%d", r );
        _DSEXCEPTION( "JSON parse : failed with code " + dsstring( comment ) );
    }
}

void JSONParser::AnalyzeTokens( ObjectContentEventHandler* p_object_handler, ArrayContentEventHandler* p_array_handler, StringContentEventHandler* p_string_handler )
{
    m_index = 0;
    int size = GetTokenSize( m_index );
    
    if( JSMN_OBJECT == GetTokenType( m_index ) )
    {
        m_index++;

        for( int i = 0; i < size; i++ )
        {
            recurs_analyze( "<root>", p_object_handler, p_array_handler, p_string_handler );
        }        
    }
    else
    {
        _DSEXCEPTION( "JSON parse : unexpected type for token 0" );
    }    
}

void JSONParser::recurs_analyze( const std::string& p_owner_id, ObjectContentEventHandler* p_object_handler, ArrayContentEventHandler* p_array_handler, StringContentEventHandler* p_string_handler )
{
    dsstring id;
    GetTokenString( m_index, id );
    m_index++;

    int content_type = GetTokenType( m_index );
    int content_size = GetTokenSize( m_index );

    switch( content_type )
    {
        case JSMN_OBJECT:
        {
            (*p_object_handler)( p_owner_id, id );
            m_index++;
            
            for( int i = 0; i < content_size; i++ )
            {
                recurs_analyze( id, p_object_handler, p_array_handler, p_string_handler );
            }
        }
        break;

        case JSMN_ARRAY:
        {
            (*p_array_handler)( p_owner_id, id );
            m_index++;


            for( int i = 0; i < content_size; i++ )
            {
                
                int sub_content_type = GetTokenType( m_index );
                int sub_content_size = GetTokenSize( m_index );

                if( JSMN_OBJECT == sub_content_type )
                {
                    char comment[32];
                    sprintf( comment, "%s_%d", id.c_str(), i );

                    (*p_object_handler)( id, comment );
                    m_index++;

                    for( int j = 0; j < sub_content_size; j++ )
                    {
                        recurs_analyze( comment, p_object_handler, p_array_handler, p_string_handler );
                    }                    
                }
                else
                {
                    // exception ici
                }
            }
        }
        break;

        case JSMN_STRING:
        {
            dsstring value;
               
            GetTokenString( m_index, value );
            (*p_string_handler)( p_owner_id, id, value );
            m_index++;
        }
        break;
    }
}