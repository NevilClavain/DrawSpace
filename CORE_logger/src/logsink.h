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

#include "ds_types.h"
#include "logoutput.h"

namespace DrawSpace
{
    namespace Logger
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

            Sink( const dsstring& p_name, Configuration* p_conf = nullptr );
            ~Sink(void) = default;

            void setCurrentLevel( Level p_level );
            void setState( bool p_state );

            void logIt( Level p_level, const dsstring& p_trace );

            void registerOutput( Output* p_output );

            void getName( dsstring& p_name ) const;
            void setConfiguration( Logger::Configuration* p_conf );

        private:

            Level                   m_current_level{ Level::LEVEL_FATAL };
            bool                    m_state{ false };

            Output*                 m_output{ nullptr };
            Logger::Configuration*  m_conf{ nullptr };

            dsstring                m_name;      
        };
    }
}


