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

#include <map>
#include <vector>
#include <functional>

#include "ds_types.h"
#include "singleton.h"
#include "logsink.h"

namespace DrawSpace
{
    namespace Logger
    {
        // fwd decl
        class Output;
        
        class Configuration : public Commons::Singleton<Configuration>
        {
        public:
            Configuration(void);
            ~Configuration( void ) = default;

            void                    registerSink( Sink* p_sink );
            void                    updateTick( void );
            LONGLONG                getLastTick( void ) const;

            using ParserCallback = std::function<void(const dsstring&, long, const std::vector<dsstring>&)>;

            ParserCallback getParserCallback(void) const;

        private:                   
            std::map<dsstring, std::unique_ptr<Output>>     m_outputs;

            using SinkInfos = std::tuple<Sink*, bool, Sink::Level, Output*>;

            std::map<dsstring, SinkInfos>                   m_sinks_infos;

            LARGE_INTEGER                                   m_base_tick;
            LARGE_INTEGER                                   m_last_tick;
            LARGE_INTEGER                                   m_freq;

            static void on_new_line(const dsstring& p_line, long p_line_num, const std::vector<dsstring>& p_words);              
        };
    }
}

