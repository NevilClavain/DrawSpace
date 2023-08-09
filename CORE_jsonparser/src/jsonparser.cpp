/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

using namespace DrawSpace;

JSONParser::JSONParser( void )
{
	jsmn_init( &m_parser );
}

int JSONParser::parse( const dsstring& p_str )
{
	m_parse_success = false;
    const auto r{ jsmn_parse(&m_parser, p_str.c_str(), p_str.size(), m_tokens, max_tokens) };
	if( r > -1 )
	{
		m_nb_tokens = r;
		m_parse_success = true;
		m_text = p_str;
	}
	return r;
}

int JSONParser::getTokenType( int p_index ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _DSEXCEPTION("JSONParser::GetTokenType");
	}

	return m_tokens[p_index].type;
}

void JSONParser::getTokenString( int p_index, dsstring& p_out_tokentext ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _DSEXCEPTION("JSONParser::GetTokenString");
	}

    const auto start{ m_tokens[p_index].start };
    const auto end{ m_tokens[p_index].end };

	p_out_tokentext = m_text.substr( start, end - start );
}

int JSONParser::getTokenSize( int p_index ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _DSEXCEPTION("JSONParser::GetTokenSize");
	}

	return m_tokens[p_index].size;
}

void JSONParser::parseFromFile( const dsstring& p_filepath )
{
    long fsize;
    auto content{ DrawSpace::File::loadAndAllocBinaryFile(p_filepath, &fsize) };
    if( !content )
    {
        _DSEXCEPTION( "Cannot open JSON file : " + p_filepath );
    }

    const auto text{ new char[fsize + 1] };

    memcpy( text, content, fsize );
    text[fsize] = 0;

    const auto r{ parse(static_cast<char*>(text)) };

    _DRAWSPACE_DELETE_N_( content );
    delete[] text;

    if( r < 0 )
    {
        _DSEXCEPTION("JSON parse : failed with code " + std::to_string(r) );
    }
}

void JSONParser::analyzeTokens( UserData* p_user_data, ObjectContentEventHandler* p_object_handler, ArrayContentEventHandler* p_array_handler, ArrayObjectContentEventHandler* p_array_object_handler, StringContentEventHandler* p_string_handler, NumericContentEventHandler* p_num_handler )
{
    m_index = 0;
    const auto size{ getTokenSize(m_index) };
    
    if( JSMN_OBJECT == getTokenType( m_index ) )
    {
        m_index++;
        for( int i = 0; i < size; i++ )
        {
            recurs_analyze( p_user_data, "<root>", p_object_handler, p_array_handler, p_array_object_handler, p_string_handler, p_num_handler );
        }        
    }
    else
    {
        _DSEXCEPTION( "JSON parse : unexpected type for token 0" );
    }    
}

JSONParser::UserData* JSONParser::recurs_analyze( JSONParser::UserData* p_user_data, const std::string& p_owner_id, ObjectContentEventHandler* p_object_handler, ArrayContentEventHandler* p_array_handler, ArrayObjectContentEventHandler* p_array_object_handler, StringContentEventHandler* p_string_handler, NumericContentEventHandler* p_num_handler )
{
    dsstring id;
    getTokenString( m_index, id );
    m_index++;

    const auto content_type{ getTokenType(m_index) };
    const auto content_size{ getTokenSize(m_index) };

    switch( content_type )
    {
        case JSMN_OBJECT:
        {
            JSONParser::UserData* sub_user_data = (*p_object_handler)( p_user_data, p_owner_id, id, ParseState::JSON_NODE_PARSE_BEGIN );
            m_index++;
            
            for( int i = 0; i < content_size; i++ )
            {
                recurs_analyze( sub_user_data, id, p_object_handler, p_array_handler, p_array_object_handler, p_string_handler, p_num_handler );
            }

            (*p_object_handler)( sub_user_data, p_owner_id, id, ParseState::JSON_NODE_PARSE_END );
        }
        break;

        case JSMN_ARRAY:
        {
            JSONParser::UserData* sub_user_data = (*p_array_handler)( p_user_data, p_owner_id, id, ParseState::JSON_NODE_PARSE_BEGIN );
            m_index++;

            for( int i = 0; i < content_size; i++ )
            {                
                const auto sub_content_type{ getTokenType(m_index) };
                const auto sub_content_size{ getTokenSize(m_index) };

                if( JSMN_OBJECT == sub_content_type )
                {
                    sub_user_data = (*p_array_object_handler)( sub_user_data, id, i, ParseState::JSON_NODE_PARSE_BEGIN );
                    m_index++;


                    const dsstring comment{ id + "_" + std::to_string(i) };
                    for( int j = 0; j < sub_content_size; j++ )
                    {
                        recurs_analyze( sub_user_data, comment, p_object_handler, p_array_handler, p_array_object_handler, p_string_handler, p_num_handler );
                    }

                    sub_user_data = (*p_array_object_handler)( sub_user_data, id, i, ParseState::JSON_NODE_PARSE_END );
                }
                else
                {
                    _DSEXCEPTION( "JSON Array content must be a JSON Object" )
                    // exception ici
                }
            }

            sub_user_data = (*p_array_handler)( p_user_data, p_owner_id, id, ParseState::JSON_NODE_PARSE_END );
        }
        break;

        case JSMN_STRING:
        {
            dsstring value;
               
            getTokenString( m_index, value );
            (*p_string_handler)( p_user_data, p_owner_id, id, value );
            m_index++;
        }
        break;

        case JSMN_PRIMITIVE:
        {
            dsstring value;
                       
            getTokenString( m_index, value );

            dsreal fval = std::stof( value );
            (*p_num_handler)( p_user_data, p_owner_id, id, fval );
            m_index++;
        }
        break;
    }

    return NULL;
}
