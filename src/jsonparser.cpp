
#include "jsonparser.h"

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