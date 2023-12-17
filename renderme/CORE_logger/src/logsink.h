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

#include "logoutput.h"

namespace renderMe
{
    namespace core
    {
        namespace logger
        {
            class Configuration;

            struct Sink
            {
            public:
                enum class Level
                {
                    LEVEL_FATAL,
                    LEVEL_ERROR,
                    LEVEL_WARN,
                    LEVEL_DEBUG,
                    LEVEL_TRACE,
                };

                Sink(const std::string& p_name, Configuration* p_conf = nullptr);

                Sink() = delete;
                Sink(const Sink&) = delete;
                Sink(Sink&&) = delete;
                Sink& operator=(const Sink& t) = delete;

                ~Sink(void) = default;

                void setCurrentLevel(Level p_level);
                void setState(bool p_state);

                void logIt(Level p_level, const std::string& p_trace);

                void registerOutput(Output* p_output);

                std::string getName() const;
                void setConfiguration(Configuration* p_conf);

            private:

                Level                   m_current_level { Level::LEVEL_FATAL };
                bool                    m_state         { false };

                Output*                 m_output        { nullptr };
                Configuration*          m_conf          { nullptr };

                std::string             m_name;
            };
        }
    }
}


