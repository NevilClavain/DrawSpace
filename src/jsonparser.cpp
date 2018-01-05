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
