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

	int			    m_nb_tokens; // significatif seulement apr�s avoir appel� Parse() sans erreur;
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
