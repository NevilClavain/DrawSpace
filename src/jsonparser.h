/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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
#include "callback.h"

namespace DrawSpace
{
namespace Utils
{
class JSONParser
{
public:

    using ParseState = enum
    {
        JSON_NODE_PARSE_BEGIN,
        JSON_NODE_PARSE_END,
    };

    struct UserData abstract {};

    template<typename T>
    struct UserDataImpl : public UserData
    {
        T m_data;
    };

    using ObjectContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const dsstring&, const dsstring&, ParseState>;
    using ArrayObjectContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const dsstring&, int, ParseState>;
    using ArrayContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const dsstring&, const dsstring&, ParseState>;
    using StringContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const dsstring&, const dsstring&, const dsstring&>;
    using NumericContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const dsstring&, const dsstring&, dsreal>;
    


protected:
	static const int max_tokens = 1024;

	jsmn_parser     m_parser;

	jsmntok_t	    m_tokens[max_tokens];

	int			    m_nb_tokens; // significatif seulement apres avoir appele Parse() sans erreur;
	bool		    m_parse_success;

	std::string     m_text;

    int             m_index;

    UserData*   recurs_analyze( UserData* p_user_data, const std::string& p_owner_id, ObjectContentEventHandler* p_object_handler, ArrayContentEventHandler* p_array_handler, ArrayObjectContentEventHandler* p_array_object_handler, StringContentEventHandler* p_string_handler, NumericContentEventHandler* p_num_handler );

public:
	JSONParser( void );
	~JSONParser( void );

	int		    Parse( const dsstring& p_str );
    void        ParseFromFile( const dsstring& p_filepath );
	int		    GetTokenType( int p_index );
	int		    GetTokenSize( int p_index );
	void	    GetTokenString( int p_index, dsstring& p_out_tokentext );

    void        AnalyzeTokens( UserData* p_user_data, ObjectContentEventHandler* p_object_handler, ArrayContentEventHandler* p_array_handler, ArrayObjectContentEventHandler* p_array_object_handler, StringContentEventHandler* p_string_handler, NumericContentEventHandler* p_num_handler );

};
}
}

#endif
