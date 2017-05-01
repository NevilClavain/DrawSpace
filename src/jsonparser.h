
#ifndef _JSONPARSER_H_
#define _JSONPARSER_H_

#include <jsmn.h>
#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Utils
{
class JSONParser
{
protected:
	static const int max_tokens = 1024;

	jsmn_parser     m_parser;

	jsmntok_t	    m_tokens[max_tokens];

	int			    m_nb_tokens; // significatif seulement après avoir appelé Parse() sans erreur;
	bool		    m_parse_success;

	std::string     m_text;

public:
	JSONParser( void );
	~JSONParser( void );

	int		    Parse( const dsstring& p_str );
    void        ParseFromFile( const dsstring& p_filepath );
	int		    GetTokenType( int p_index );
	int		    GetTokenSize( int p_index );
	void	    GetTokenString( int p_index, dsstring& p_out_tokentext );

};
}
}

#endif