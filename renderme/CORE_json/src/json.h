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
#include <functional>

#include "eventsource.h"

namespace renderMe
{
    namespace core
    {

        enum class JSONEvent
        {
            OBJECT_BEGIN,
            ARRAY_BEGIN,
            OBJECT_END,
            ARRAY_END,
            STRING,
            PRIMITIVE
        };

        class Json : public renderMe::property::EventSource<JSONEvent, const std::string&, int, const std::string&>
        {
        public:

            //static constexpr int unused{ -1 };

            //using ParserCallback = std::function<void(Event, const std::string&, int, const std::string&)>;

            Json(void);
            ~Json(void) = default;

            int		    parse(const std::string& p_str);

            //void        setCallback(const ParserCallback& p_cb);

        private:

            static constexpr    int max_tokens{ 1024 };

            jsmntok_t	        m_tokens[max_tokens];
            jsmn_parser         m_parser;

            int			        m_nb_tokens{ -1 }; // significatif seulement apres avoir appele Parse() sans erreur;
            bool		        m_parse_success{ false };

            std::string         m_text;

            int                 m_index{ -1 };

            //ParserCallback      m_parserCallback{ [](Event, const std::string&, int p_index, const std::string&) {} };


            jsmntype_t		    get_token_type(int p_index) const;
            size_t		        get_token_size(int p_index) const;
            std::string	        get_token_string(int p_index) const;

            void                analyze_tokens(void);
            void                recurs_analyze(void);

        };

    }

}

