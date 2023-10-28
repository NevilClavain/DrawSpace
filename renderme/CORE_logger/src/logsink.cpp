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

#include <sstream>
#include <iomanip> // setprecision
#include <unordered_map>

#include "logsink.h"
#include "logconf.h"

using namespace renderMe::core;

logger::Sink::Sink( const std::string& p_name, logger::Configuration* p_conf ) :
m_name( p_name ),
m_conf( p_conf )
{
    if( p_conf )
    {
        p_conf->registerSink( this );
    }
}

void logger::Sink::setCurrentLevel(logger::Sink::Level p_level )
{
    m_current_level = p_level;
}

void logger::Sink::setState( bool p_state )
{
    m_state = p_state;
}

void logger::Sink::logIt( Level p_level, const std::string& p_trace )
{
    if( p_level <= m_current_level && m_state && m_output )
    {        
        static const std::unordered_map<Level, std::string> lvl_to_string
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

        std::string timestamp;
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

        std::string final_trace = timestamp + std::string( " " ) + thread_id + std::string( " " ) + m_name + std::string( " " ) + level + std::string( " " ) + std::string( "[ " ) + p_trace + std::string( " ]" ) + std::string( "\n" );
        m_output->logIt( final_trace );
    }

    if( m_conf )
    {
        m_conf->updateTick();
    }
}

void logger::Sink::registerOutput(logger::Output* p_output )
{
    m_output = p_output;
}

void logger::Sink::getName( std::string& p_name ) const
{
    p_name = m_name;
}

void logger::Sink::setConfiguration(logger::Configuration* p_conf )
{
    m_conf = p_conf;
}
