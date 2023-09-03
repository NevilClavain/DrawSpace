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

#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <functional>

#include "singleton.h"
#include "logsink.h"
#include "json.h"

namespace renderMe
{
    namespace core
    {
        namespace logger
        {
            // fwd decl
            class Output;

            class Configuration : public Singleton<Configuration>
            {
            public:
                Configuration(void);
                ~Configuration(void) = default;

                void                    registerSink(Sink* p_sink);
                void                    updateTick(void);
                LONGLONG                getLastTick(void) const;

                //using ParserCallback = std::function<void(renderMe::core::json::Event, const std::string&, int, const std::string&)>;

                json::ParserCallback          getParserCallback(void);

            private:
                std::map<std::string, std::unique_ptr<Output>>      m_outputs;

                using SinkInfos = std::tuple<Sink*, bool, Sink::Level, Output*>;
                std::map<std::string, SinkInfos>                    m_sinks_infos;

                LARGE_INTEGER                                       m_base_tick;
                LARGE_INTEGER                                       m_last_tick;
                LARGE_INTEGER                                       m_freq;

                //// JSON parsing working members

                enum class ParsingState
                {
                    IDLE,
                    RECORD_CONFIG,
                    RECORD_LOGGER
                };

                ParsingState                                        m_parsing_state{ ParsingState::IDLE };

                std::string                                         m_mem_output_type;
                std::string                                         m_mem_output_id;
                std::string                                         m_mem_output_path;

                std::string                                         m_mem_logger_source;
                Sink::Level                                         m_mem_logger_level;
                bool                                                m_mem_logger_state;
                std::string                                         m_mem_logger_output;


                //static void on_json_event(renderMe::core::json::Event p_event, const std::string& p_id, const std::string& p_value);
            };
        }
    }
}

