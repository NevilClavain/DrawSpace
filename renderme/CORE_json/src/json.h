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

#pragma once

#include <jsmn.h>
#include <string>

namespace renderMe
{
    namespace core
    {

        class json
        {
        public:

            enum class ParseState
            {
                JSON_NODE_PARSE_BEGIN,
                JSON_NODE_PARSE_END
            };

            //struct UserData abstract {};



            /*
            using ObjectContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const std::string&, const std::string&, ParseState>;
            using ArrayObjectContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const std::string&, int, ParseState>;
            using ArrayContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const std::string&, const std::string&, ParseState>;
            using StringContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const std::string&, const std::string&, const std::string&>;
            using NumericContentEventHandler = DrawSpace::Core::BaseCallback4<UserData*, UserData*, const std::string&, const std::string&, dsreal>;
            */

            json(void);
            ~json(void) = default;

            int		    parse(const std::string& p_str);



        private:

            static constexpr    int max_tokens{ 1024 };

            jsmntok_t	        m_tokens[max_tokens];
            jsmn_parser         m_parser;

            int			        m_nb_tokens{ -1 }; // significatif seulement apres avoir appele Parse() sans erreur;
            bool		        m_parse_success{ false };

            std::string         m_text;

            int                 m_index{ -1 };

            int		            get_token_type(int p_index) const;
            int		            get_token_size(int p_index) const;
            void	            get_token_string(int p_index, std::string& p_out_tokentext) const;

            void                analyzeTokens(/*UserData* p_user_data
                                    ,ObjectContentEventHandler* p_object_handler,
                                    ArrayContentEventHandler* p_array_handler,
                                    ArrayObjectContentEventHandler* p_array_object_handler,
                                    StringContentEventHandler* p_string_handler,
                                    NumericContentEventHandler* p_num_handler
                                    */);

            void recurs_analyze(
                                        /*
                                        UserData* p_user_data, 
                                        const std::string& p_owner_id, 
                                        ObjectContentEventHandler* p_object_handler, 
                                        ArrayContentEventHandler* p_array_handler, 
                                        ArrayObjectContentEventHandler* p_array_object_handler, 
                                        StringContentEventHandler* p_string_handler, 
                                        NumericContentEventHandler* p_num_handler
            
                                    */);

        };

    }

}

