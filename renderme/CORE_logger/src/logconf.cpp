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

#include "logconf.h"

#include "logoutput.h"
#include "logoutputfile.h"
#include "exceptions.h"

using namespace renderMe::core;

logger::Configuration::Configuration( void )
{
    QueryPerformanceFrequency( &m_freq );
    QueryPerformanceCounter( &m_base_tick );
    m_last_tick = m_base_tick;

    m_cb = [&, this](JSONEvent p_event, const std::string& p_id, int p_index, const std::string& p_value)
    {
        switch (p_event)
        {
        case renderMe::core::JSONEvent::OBJECT_BEGIN:

            if ("outputs" == p_id)
            {
                this->m_parsing_state = ParsingState::RECORD_CONFIG;
            }
            else if ("loggers" == p_id)
            {
                this->m_parsing_state = ParsingState::RECORD_LOGGER;
            }
            break;

        case renderMe::core::JSONEvent::OBJECT_END:

            if ("outputs" == p_id)
            {
                // create the output
                if ("file" == m_mem_output_type)
                {
                    // file output

                    this->m_outputs[m_mem_output_id] = std::make_unique<OutputFile>(m_mem_output_path);
                    const auto& of{ this->m_outputs[m_mem_output_id] };
                    of.get()->setFlushPeriod(0);
                }
                // no other type of output for now

            }
            else if ("loggers" == p_id)
            {
                const auto output{ this->m_outputs.at(m_mem_logger_output).get() };

                if (this->m_sinks_infos.count(this->m_mem_logger_source))
                {
                    auto& sink_info{ this->m_sinks_infos[this->m_mem_logger_source] };

                    const auto sink{ std::get<0>(this->m_sinks_infos.at(this->m_mem_logger_source)) };

                    sink->setCurrentLevel(m_mem_logger_level);
                    sink->setState(m_mem_logger_state);
                    sink->registerOutput(output);

                    std::get<1>(sink_info) = m_mem_logger_state;
                    std::get<2>(sink_info) = m_mem_logger_level;
                    std::get<3>(sink_info) = output;
                }
                else
                {
                    this->m_sinks_infos[this->m_mem_logger_source] = std::make_tuple(nullptr, m_mem_logger_state, m_mem_logger_level, output);
                }
            }
            break;

            /* not ussed
            case renderMe::core::json::Event::ARRAY_BEGIN:

                break;

            case renderMe::core::json::Event::ARRAY_END:

                break;
            */

        case renderMe::core::JSONEvent::STRING:

            if (ParsingState::RECORD_CONFIG == this->m_parsing_state)
            {
                if ("type" == p_id)
                {
                    m_mem_output_type = p_value;
                }
                else if ("id" == p_id)
                {
                    m_mem_output_id = p_value;
                }
                else if ("path" == p_id)
                {
                    m_mem_output_path = p_value;
                }
            }
            else if (ParsingState::RECORD_LOGGER == this->m_parsing_state)
            {
                if ("source" == p_id)
                {
                    m_mem_logger_source = p_value;
                }
                else if ("level" == p_id)
                {
                    static const std::map<std::string, Sink::Level> levelTranslation
                    {
                        { "FATAL", Sink::Level::LEVEL_FATAL},
                        { "ERROR", Sink::Level::LEVEL_ERROR},
                        { "WARN", Sink::Level::LEVEL_WARN},
                        { "DEBUG", Sink::Level::LEVEL_DEBUG},
                        { "TRACE", Sink::Level::LEVEL_TRACE},
                    };

                    m_mem_logger_level = levelTranslation.at(p_value);
                }
                else if ("state" == p_id)
                {
                    static const std::map<std::string, bool> stateTranslation
                    {
                        { "on", true},
                        { "off", false},
                    };

                    m_mem_logger_state = stateTranslation.at(p_value);
                }
                else if ("output" == p_id)
                {
                    m_mem_logger_output = p_value;
                }
            }
            break;

            /* not used
                case renderMe::core::json::Event::PRIMITIVE:
                    break;
            */
        }


    };
}



void logger::Configuration::registerSink( Sink* p_sink )
{
    std::string name;
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

void logger::Configuration::updateTick( void )
{
    QueryPerformanceCounter( &m_last_tick );
}

LONGLONG logger::Configuration::getLastTick( void ) const
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
