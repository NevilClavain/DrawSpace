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
#include <optional>

#include "exceptions.h"

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

        class DefaultUserData {};

        template<typename UserDataT=class DefaultUserData>
        class Json : public renderMe::property::EventSource<JSONEvent, const std::string&, int, const std::string&, const std::optional<UserDataT*>&>
        {
        public:

            Json(void)
            {
                jsmn_init(&m_parser);
            }

            ~Json(void) = default;

            int parse(const std::string& p_str, std::optional<UserDataT*> p_userData = std::nullopt)
            {
                m_parse_success = false;
                const auto r{ jsmn_parse(&m_parser, p_str.c_str(), p_str.size(), m_tokens, max_tokens) };
                if (r > -1)
                {
                    m_nb_tokens = r;
                    m_parse_success = true;
                    m_text = p_str;

                    analyze_tokens(p_userData);                    
                }
                return r;
            }

        private:

            static constexpr    int max_tokens{ 1024 };

            jsmntok_t	        m_tokens[max_tokens];
            jsmn_parser         m_parser;

            int			        m_nb_tokens{ -1 }; // significatif seulement apres avoir appele Parse() sans erreur;
            bool		        m_parse_success{ false };

            std::string         m_text;

            int                 m_index{ -1 };

            jsmntype_t get_token_type(int p_index) const
            {
                if (!m_parse_success || p_index > m_nb_tokens - 1)
                {
                    _EXCEPTION("Json::GetTokenType");
                }
                return m_tokens[p_index].type;
            }

            size_t get_token_size(int p_index) const
            {
                if (!m_parse_success || p_index > m_nb_tokens - 1)
                {
                    _EXCEPTION("Json::GetTokenSize");
                }

                return m_tokens[p_index].size;
            }

            std::string	get_token_string(int p_index) const
            {
                if (!m_parse_success || p_index > m_nb_tokens - 1)
                {
                    _EXCEPTION("Json::GetTokenString");
                }

                const auto start{ m_tokens[p_index].start };
                const auto end{ m_tokens[p_index].end };

                return m_text.substr(start, end - start);
            }

            void analyze_tokens(const std::optional<UserDataT*>& p_userData)
            {
                m_index = 0;
                const auto size{ get_token_size(m_index) };

                if (JSMN_OBJECT == get_token_type(m_index))
                {
                    m_index++;
                    for (size_t i = 0; i < size; i++)
                    {
                        recurs_analyze(p_userData);
                    }
                }
                else
                {
                    _EXCEPTION("JSON parse : unexpected type for token 0");
                }
            }

            void recurs_analyze(const std::optional<UserDataT*>& p_userData)
            {
                const auto id{ get_token_string(m_index) };
                m_index++;

                const auto content_type{ get_token_type(m_index) };
                const auto content_size{ get_token_size(m_index) };

                switch (content_type)
                {
                case JSMN_OBJECT:
                {
                    for (const auto& call : m_callbacks)
                    {
                        call(JSONEvent::OBJECT_BEGIN, id, -1, "", p_userData);
                    }
                    m_index++;

                    for (size_t i = 0; i < content_size; i++)
                    {
                        recurs_analyze(p_userData);
                    }

                    for (const auto& call : m_callbacks)
                    {
                        call(JSONEvent::OBJECT_END, id, -1, "", p_userData);
                    }
                }
                break;

                case JSMN_ARRAY:
                {
                    for (const auto& call : m_callbacks)
                    {
                        call(JSONEvent::ARRAY_BEGIN, id, -1, "", p_userData);
                    }
                    m_index++;

                    for (size_t i = 0; i < content_size; i++)
                    {
                        const auto sub_content_type{ get_token_type(m_index) };
                        const auto sub_content_size{ get_token_size(m_index) };

                        if (JSMN_OBJECT == sub_content_type)
                        {
                            m_index++;

                            for (const auto& call : m_callbacks)
                            {
                                call(JSONEvent::OBJECT_BEGIN, id, i, "", p_userData);
                            }


                            for (size_t j = 0; j < sub_content_size; j++)
                            {
                                recurs_analyze(p_userData);
                            }

                            for (const auto& call : m_callbacks)
                            {
                                call(JSONEvent::OBJECT_END, id, i, "", p_userData);
                            }
                        }
                        else
                        {
                            _EXCEPTION("JSON Array content must be a JSON Object")
                        }
                    }

                    for (const auto& call : m_callbacks)
                    {
                        call(JSONEvent::ARRAY_END, id, -1, "", p_userData);
                    }
                }
                break;

                case JSMN_STRING:
                {
                    const auto value{ get_token_string(m_index) };
                    for (const auto& call : m_callbacks)
                    {
                        call(JSONEvent::STRING, id, -1, value, p_userData);
                    }

                    m_index++;
                }
                break;

                case JSMN_PRIMITIVE:
                {
                    const auto value{ get_token_string(m_index) };
                    for (const auto& call : m_callbacks)
                    {
                        call(JSONEvent::PRIMITIVE, id, -1, value, p_userData);
                    }

                    m_index++;
                }
                break;
                }
            }

        };

    }

}

