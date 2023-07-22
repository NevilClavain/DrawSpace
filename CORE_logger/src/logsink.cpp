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

#include <sstream>
#include <iomanip> // setprecision

#include "logsink.h"
#include "logconf.h"

using namespace DrawSpace;

Logger::Sink::Sink( const dsstring& p_name, Logger::Configuration* p_conf ) : 
m_name( p_name ),
m_conf( p_conf )
{
    if( p_conf )
    {
        p_conf->registerSink( this );
    }
}

void Logger::Sink::setCurrentLevel( Logger::Sink::Level p_level )
{
    m_current_level = p_level;
}

void Logger::Sink::setState( bool p_state )
{
    m_state = p_state;
}

void Logger::Sink::logIt( Level p_level, const dsstring& p_trace )
{
    if( p_level <= m_current_level && m_state && m_output )
    {        
        static const std::map<Level, dsstring> lvl_to_string
        {
            { Level::LEVEL_FATAL, "FATAL"},
            { Level::LEVEL_ERROR, "ERROR"},
            { Level::LEVEL_WARN, "WARN"},
            { Level::LEVEL_DEBUG, "DEBUG"},
            { Level::LEVEL_TRACE, "TRACE"},

        };

        const auto level{ lvl_to_string.at(p_level) };
       
        std::stringstream stream;
        stream << std::hex << GetCurrentThreadId();
        const std::string thread_id( std::string("[" ) + stream.str() + std::string("]"));

        dsstring timestamp;
        if( m_conf )
        {
            const auto timestamp_in_second{ m_conf->getLastTick() / 1000000.0 };            
            const auto precision{ 6 };
            std::stringstream stream;
            stream << std::fixed << std::setprecision(precision) << timestamp_in_second;
            timestamp = stream.str();
        }
        else
        {
            timestamp = "??????????";            
        }

        dsstring final_trace = timestamp + dsstring( " " ) + thread_id + dsstring( " " ) + m_name + dsstring( " " ) + level + dsstring( " " ) + dsstring( "[ " ) + p_trace + dsstring( " ]" ) + dsstring( "\n" );
        m_output->logIt( final_trace );
    }

    if( m_conf )
    {
        m_conf->updateTick();
    }
}

void Logger::Sink::registerOutput( Logger::Output* p_output )
{
    m_output = p_output;
}

void Logger::Sink::getName( dsstring& p_name ) const
{
    p_name = m_name;
}

void Logger::Sink::setConfiguration( Logger::Configuration* p_conf )
{
    m_conf = p_conf;
}
