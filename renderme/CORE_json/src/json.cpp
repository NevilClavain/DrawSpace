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

#include "Json.h"

/*
#include "memalloc.h"
#include "file.h"
*/

#include "exceptions.h"

using namespace renderMe::core;

Json::Json( void )
{
	jsmn_init( &m_parser );
}

int Json::parse( const std::string& p_str )
{
	m_parse_success = false;
    const auto r{ jsmn_parse(&m_parser, p_str.c_str(), p_str.size(), m_tokens, max_tokens) };
	if( r > -1 )
	{
		m_nb_tokens = r;
		m_parse_success = true;
		m_text = p_str;

        analyze_tokens();
	}
	return r;
}

/*
void Json::setCallback(const ParserCallback& p_cb)
{
    m_parserCallback = p_cb;
}
*/

jsmntype_t Json::get_token_type( int p_index ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _EXCEPTION("Json::GetTokenType");
	}

	return m_tokens[p_index].type;
}

std::string Json::get_token_string( int p_index ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _EXCEPTION("Json::GetTokenString");
	}

    const auto start{ m_tokens[p_index].start };
    const auto end{ m_tokens[p_index].end };

	return m_text.substr( start, end - start );
}

size_t Json::get_token_size( int p_index ) const
{
	if( !m_parse_success || p_index > m_nb_tokens - 1 )
	{
        _EXCEPTION("Json::GetTokenSize");
	}

	return m_tokens[p_index].size;
}

void Json::analyze_tokens(void)
{
    m_index = 0;
    const auto size{ get_token_size(m_index) };
    
    if( JSMN_OBJECT == get_token_type( m_index ) )
    {
        m_index++;
        for( size_t i = 0; i < size; i++ )
        {
            recurs_analyze();
        }        
    }
    else
    {
        _EXCEPTION( "JSON parse : unexpected type for token 0" );
    }    
}

void Json::recurs_analyze(void)
{
    const auto id{ get_token_string(m_index) };
    m_index++;

    const auto content_type{ get_token_type(m_index) };
    const auto content_size{ get_token_size(m_index) };

    switch( content_type )
    {
        case JSMN_OBJECT:
        {
            for (const auto& call : m_callbacks)
            {
                call(JSONEvent::OBJECT_BEGIN, id, -1, "");
            }
            m_index++;

            for( size_t i = 0; i < content_size; i++ )
            {
                recurs_analyze();
            }

            for (const auto& call : m_callbacks)
            {
                call(JSONEvent::OBJECT_END, id, -1, "");
            }
        }
        break;

        case JSMN_ARRAY:
        {
            for (const auto& call : m_callbacks)
            {
                call(JSONEvent::ARRAY_BEGIN, id, -1, "");
            }
            m_index++;

            for( size_t i = 0; i < content_size; i++ )
            {                
                const auto sub_content_type{ get_token_type(m_index) };
                const auto sub_content_size{ get_token_size(m_index) };

                if( JSMN_OBJECT == sub_content_type )
                {
                    m_index++;

                    for (const auto& call : m_callbacks)
                    {
                        call(JSONEvent::OBJECT_BEGIN, id, i, "");
                    }


                    for( size_t j = 0; j < sub_content_size; j++ )
                    {
                        recurs_analyze();
                    }

                    for (const auto& call : m_callbacks)
                    {
                        call(JSONEvent::OBJECT_END, id, i, "");
                    }
                }
                else
                {
                    _EXCEPTION( "JSON Array content must be a JSON Object" )
                }
            }

            for (const auto& call : m_callbacks)
            {
                call(JSONEvent::ARRAY_END, id, -1, "");
            }
        }
        break;

        case JSMN_STRING:
        {      
            const auto value{ get_token_string(m_index) };
            for (const auto& call : m_callbacks)
            {
                call(JSONEvent::STRING, id, -1, value);
            }

            m_index++;
        }
        break;

        case JSMN_PRIMITIVE:
        {            
            const auto value{ get_token_string(m_index) };
            for (const auto& call : m_callbacks)
            {
                call(JSONEvent::PRIMITIVE, id, -1, value);
            }

            m_index++;
        }
        break;
    }
}
