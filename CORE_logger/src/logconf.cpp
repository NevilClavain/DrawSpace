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

#include "logconf.h"

#include "logoutput.h"
#include "logoutputfile.h"
#include "exceptions.h"


using namespace DrawSpace;
using namespace DrawSpace::Logger;


Logger::Configuration::Configuration( void )
{
    QueryPerformanceFrequency( &m_freq );
    QueryPerformanceCounter( &m_base_tick );
    m_last_tick = m_base_tick;
}


Logger::Configuration::ParserCallback Logger::Configuration::getParserCallback(void) const
{
    const ParserCallback cb{ &on_new_line };
    return cb;
}

void Logger::Configuration::on_new_line( const dsstring& p_line, long p_line_num, const std::vector<dsstring>& p_words )
{
    if( "logger" == p_words[0] )
    {
        if( p_words.size() < 5 )
        {
            _DSEXCEPTION("log configuration file : missing params for sink, line " + std::to_string(p_line_num));
        }

        if( 0 == Configuration::getInstance()->m_outputs.count( p_words[4] ) )
        {
            _DSEXCEPTION("log configuration file : unknown output name for sink, line " + std::to_string(p_line_num));
        }

        const auto output{ Configuration::getInstance()->m_outputs[p_words[4]].get() };

        bool state{ false };
        if ("on" == p_words[3])
        {
            state = true;
        }
        else if ("off" == p_words[3])
        {
            state = false;
        }
        else
        {
            _DSEXCEPTION("log configuration file : bad state for sink, line " + std::to_string(p_line_num));
        }

        static const std::map<dsstring, Sink::Level> str_to_level =
        {
            { "TRACE", Sink::Level::LEVEL_TRACE},
            { "DEBUG", Sink::Level::LEVEL_DEBUG},
            { "WARN", Sink::Level::LEVEL_WARN},
            { "ERROR", Sink::Level::LEVEL_ERROR},
            { "FATAL", Sink::Level::LEVEL_FATAL}
        };

        const auto lvl{ str_to_level.at(p_words[2]) };

       
        if (Configuration::getInstance()->m_sinks_infos.count(p_words[1]) > 0)
        {
            auto& sink_info{ Configuration::getInstance()->m_sinks_infos[p_words[1]] };

            const auto sink{ std::get<0>( Configuration::getInstance()->m_sinks_infos.at(p_words[1]) ) };

            sink->setCurrentLevel(lvl);
            sink->setState(state);
            sink->registerOutput(output);

            std::get<1>(sink_info) = state;
            std::get<2>(sink_info) = lvl;
            std::get<3>(sink_info) = output;
        }

        else
        {
            Configuration::getInstance()->m_sinks_infos[p_words[1]] = std::make_tuple(nullptr, state, lvl, output);
        }
    }
    else if( "output" == p_words[0] )
    {
        if( p_words.size() < 5 )
        {
            _DSEXCEPTION("log configuration file : missing params for output, line " + std::to_string(p_line_num));
        }

        if( "file" == p_words[1] )
        {
            Configuration::getInstance()->m_outputs[p_words[2]] = std::make_unique<OutputFile>(p_words[3]);
            const auto& of{ Configuration::getInstance()->m_outputs[p_words[2]] };
            of.get()->setFlushPeriod(std::atoi(p_words[4].c_str()));
        }
        else
        {
            _DSEXCEPTION("log configuration file : unknown class for output, line  " + std::to_string(p_line_num));
        }
    }
}

void Logger::Configuration::registerSink( Sink* p_sink )
{
    dsstring name;
    p_sink->getName( name );

    //if( m_sinks.count( name ) > 0 )

    if (m_sinks_infos.count(name) > 0)
    {
        // entry exists
        p_sink->setCurrentLevel(std::get<2>(m_sinks_infos.at(name)));
        p_sink->setState(std::get<1>(m_sinks_infos.at(name)));
        p_sink->registerOutput(std::get<3>(m_sinks_infos.at(name)));
    }
    else
    {
        // entry does not exists, create it
        m_sinks_infos[name] = std::make_tuple(p_sink, false, Sink::Level::LEVEL_FATAL, nullptr);
    }
}

void Logger::Configuration::updateTick( void )
{
    QueryPerformanceCounter( &m_last_tick );
}

LONGLONG Logger::Configuration::getLastTick( void ) const
{
    LONGLONG elapsed{ m_last_tick.QuadPart - m_base_tick.QuadPart };

    //
    // We now have the elapsed number of ticks, along with the
    // number of ticks-per-second. We use these values
    // to convert to the number of elapsed microseconds.
    // To guard against loss-of-precision, we convert
    // to microseconds *before* dividing by ticks-per-second.
    //

    elapsed *= 1000000;
    elapsed /= m_freq.QuadPart;

    return elapsed;
}
