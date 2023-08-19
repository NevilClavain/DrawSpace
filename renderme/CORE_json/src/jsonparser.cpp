/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "jsonparser.h"

/*
#include "memalloc.h"
#include "file.h"
*/

#include "exceptions.h"

using namespace renderMe::core;

JSONParser::JSONParser( void )
{
	jsmn_init( &m_parser );
}

int JSONParser::parse( const std::string& p_str )
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

int JSONParser::get_token_type( int p_index ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _EXCEPTION("JSONParser::GetTokenType");
	}

	return m_tokens[p_index].type;
}

void JSONParser::get_token_string( int p_index, std::string& p_out_tokentext ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _EXCEPTION("JSONParser::GetTokenString");
	}

    const auto start{ m_tokens[p_index].start };
    const auto end{ m_tokens[p_index].end };

	p_out_tokentext = m_text.substr( start, end - start );
}

int JSONParser::get_token_size( int p_index ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _EXCEPTION("JSONParser::GetTokenSize");
	}

	return m_tokens[p_index].size;
}

/*
void JSONParser::parseFromFile( const std::string& p_filepath )
{
    long fsize;
    auto content{ DrawSpace::File::loadAndAllocBinaryFile(p_filepath, &fsize) };
    if( !content )
    {
        _EXCEPTION( "Cannot open JSON file : " + p_filepath );
    }

    const auto text{ new char[fsize + 1] };

    memcpy( text, content, fsize );
    text[fsize] = 0;

    const auto r{ parse(static_cast<char*>(text)) };

    _DRAWSPACE_DELETE_N_( content );
    delete[] text;

    if( r < 0 )
    {
        _EXCEPTION("JSON parse : failed with code " + std::to_string(r) );
    }
}
*/

void JSONParser::analyzeTokens( 
                                /*
                                UserData* p_user_data, 
                                ObjectContentEventHandler* p_object_handler, 
                                    ArrayContentEventHandler* p_array_handler, 
                                    ArrayObjectContentEventHandler* p_array_object_handler, 
                                    StringContentEventHandler* p_string_handler, 
                                    NumericContentEventHandler* p_num_handler*/)


{
    m_index = 0;
    const auto size{ get_token_size(m_index) };
    
    if( JSMN_OBJECT == get_token_type( m_index ) )
    {
        m_index++;
        for( int i = 0; i < size; i++ )
        {
            //recurs_analyze( p_user_data, "<root>", p_object_handler, p_array_handler, p_array_object_handler, p_string_handler, p_num_handler );

            recurs_analyze();
        }        
    }
    else
    {
        _EXCEPTION( "JSON parse : unexpected type for token 0" );
    }    
}

JSONParser::UserData* JSONParser::recurs_analyze( 
    
                                                /*
                                                    JSONParser::UserData* p_user_data, 
                                                    const std::string& p_owner_id, 
                                                        ObjectContentEventHandler* p_object_handler, 
                                                        ArrayContentEventHandler* p_array_handler, 
                                                        ArrayObjectContentEventHandler* p_array_object_handler, 
                                                        StringContentEventHandler* p_string_handler, 
                                                        NumericContentEventHandler* p_num_handler 

                                                    */)
{
    std::string id;
    get_token_string( m_index, id );
    m_index++;

    const auto content_type{ get_token_type(m_index) };
    const auto content_size{ get_token_size(m_index) };

    switch( content_type )
    {
        case JSMN_OBJECT:
        {
            //JSONParser::UserData* sub_user_data = (*p_object_handler)( p_user_data, p_owner_id, id, ParseState::JSON_NODE_PARSE_BEGIN );
            m_index++;
            
            for( int i = 0; i < content_size; i++ )
            {
                //recurs_analyze( sub_user_data, id, p_object_handler, p_array_handler, p_array_object_handler, p_string_handler, p_num_handler );

                recurs_analyze();
            }

            //(*p_object_handler)( sub_user_data, p_owner_id, id, ParseState::JSON_NODE_PARSE_END );
        }
        break;

        case JSMN_ARRAY:
        {
            //JSONParser::UserData* sub_user_data = (*p_array_handler)( p_user_data, p_owner_id, id, ParseState::JSON_NODE_PARSE_BEGIN );
            m_index++;

            for( int i = 0; i < content_size; i++ )
            {                
                const auto sub_content_type{ get_token_type(m_index) };
                const auto sub_content_size{ get_token_size(m_index) };

                if( JSMN_OBJECT == sub_content_type )
                {
                    //sub_user_data = (*p_array_object_handler)( sub_user_data, id, i, ParseState::JSON_NODE_PARSE_BEGIN );
                    m_index++;


                    const std::string comment{ id + "_" + std::to_string(i) };
                    for( int j = 0; j < sub_content_size; j++ )
                    {
                        //recurs_analyze( sub_user_data, comment, p_object_handler, p_array_handler, p_array_object_handler, p_string_handler, p_num_handler );

                        recurs_analyze();
                    }

                    //sub_user_data = (*p_array_object_handler)( sub_user_data, id, i, ParseState::JSON_NODE_PARSE_END );
                }
                else
                {
                    _EXCEPTION( "JSON Array content must be a JSON Object" )
                    // exception ici
                }
            }

            //sub_user_data = (*p_array_handler)( p_user_data, p_owner_id, id, ParseState::JSON_NODE_PARSE_END );
        }
        break;

        case JSMN_STRING:
        {
            std::string value;
               
            get_token_string( m_index, value );

            //(*p_string_handler)( p_user_data, p_owner_id, id, value );

            m_index++;
        }
        break;

        case JSMN_PRIMITIVE:
        {
            std::string value;
                       
            get_token_string( m_index, value );

            double fval = std::stof( value );

            //(*p_num_handler)( p_user_data, p_owner_id, id, fval );

            m_index++;
        }
        break;
    }

    return NULL;
}
