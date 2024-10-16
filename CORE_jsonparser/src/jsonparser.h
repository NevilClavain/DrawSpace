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

#pragma once

#include <jsmn.h>
#include "ds_types.h"
#include "callback.h"

namespace DrawSpace
{
    class JSONParser
    {
    public:


        enum class ParseState
        {
            JSON_NODE_PARSE_BEGIN,
            JSON_NODE_PARSE_END
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
    
	    JSONParser( void );
        ~JSONParser(void) = default;

	    int		    parse( const dsstring& p_str );
        void        parseFromFile( const dsstring& p_filepath );
	    int		    getTokenType( int p_index ) const;
	    int		    getTokenSize( int p_index ) const;
	    void	    getTokenString( int p_index, dsstring& p_out_tokentext ) const;

        void        analyzeTokens( UserData* p_user_data, ObjectContentEventHandler* p_object_handler, ArrayContentEventHandler* p_array_handler, ArrayObjectContentEventHandler* p_array_object_handler, StringContentEventHandler* p_string_handler, NumericContentEventHandler* p_num_handler );

    private:

        static constexpr    int max_tokens{ 1024 };

        jsmntok_t	        m_tokens[max_tokens];
        jsmn_parser         m_parser;
        
        int			        m_nb_tokens     { -1 }; // significatif seulement apres avoir appele Parse() sans erreur;
        bool		        m_parse_success { false };

        std::string         m_text;

        int                 m_index         { -1 };

        UserData* recurs_analyze(UserData* p_user_data, const std::string& p_owner_id, ObjectContentEventHandler* p_object_handler, ArrayContentEventHandler* p_array_handler, ArrayObjectContentEventHandler* p_array_object_handler, StringContentEventHandler* p_string_handler, NumericContentEventHandler* p_num_handler);

    };

}

