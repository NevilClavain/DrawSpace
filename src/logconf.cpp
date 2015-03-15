/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "logconf.h"
#include "logoutputfile.h"
#include "exceptions.h"

DrawSpace::Logger::Configuration* DrawSpace::Logger::Configuration::m_instance = NULL;

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Logger;


Logger::Configuration::Configuration( void )
{
}

Logger::Configuration::~Configuration( void )
{
    for( std::map<dsstring, Output*>::iterator it = m_outputs.begin(); it != m_outputs.end(); ++it )
    {
        delete it->second;
    }
}

Logger::Configuration* Logger::Configuration::GetInstance( void )
{
    if( !m_instance )
    {
        m_instance = new Configuration();
    }
    return m_instance;
}

void Logger::Configuration::RemoveInstance( void )
{
    if( m_instance )
    {
        delete m_instance;
    }
}

bool Logger::Configuration::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "logger" == p_words[0] )
    {
        if( p_words.size() < 5 )
        {
            char comment[128];
            sprintf( comment, "log configuration file : missing params for sink, line %d", p_line_num );
            _DSEXCEPTION( comment );   
            return false;
        }

        if( 0 == m_outputs.count( p_words[4] ) )
        {
            char comment[128];
            sprintf( comment, "log configuration file : unknown output name for sink, line %d", p_line_num );
            _DSEXCEPTION( comment );   
            return false;
        }

        SinkEntry sink_entry;

        sink_entry.output = m_outputs[p_words[4]];
        
        if( "on" == p_words[3] )
        {
            sink_entry.state = true;
        }
        else if( "off" == p_words[3] )
        {
            sink_entry.state = false;
        }
        else
        {
            char comment[128];
            sprintf( comment, "log configuration file : bad state for sink, line %d", p_line_num );
            _DSEXCEPTION( comment );   
            return false;
        }

        if( "TRACE" == p_words[2] )
        {
            sink_entry.level = Sink::LEVEL_TRACE;
        }
        else if( "DEBUG" == p_words[2] )
        {
            sink_entry.level = Sink::LEVEL_DEBUG;
        }
        else if( "WARN" == p_words[2] )
        {
            sink_entry.level = Sink::LEVEL_WARN;
        }
        else if( "ERROR" == p_words[2] )
        {
            sink_entry.level = Sink::LEVEL_ERROR;
        }
        else if( "FATAL" == p_words[2] )
        {
            sink_entry.level = Sink::LEVEL_FATAL;
        }
        else
        {
            char comment[128];
            sprintf( comment, "log configuration file : unknown level keyword for sink, line %d", p_line_num );
            _DSEXCEPTION( comment );   
            return false;
        }


        if( m_sinks.count( p_words[1] ) > 0 )
        {
            sink_entry.sink = m_sinks[p_words[1]].sink;

            sink_entry.sink->SetCurrentLevel( sink_entry.level );
            sink_entry.sink->SetState( sink_entry.state );
            sink_entry.sink->RegisterOutput( sink_entry.output );
        }
        else
        {
            sink_entry.sink = NULL;
        }

        m_sinks[p_words[1]] = sink_entry;
    }
    else if( "output" == p_words[0] )
    {
        if( p_words.size() < 5 )
        {
            char comment[128];
            sprintf( comment, "log configuration file : missing params for output, line %d", p_line_num );
            _DSEXCEPTION( comment );   
            return false;
        }

        if( "file" == p_words[1] )
        {
            OutputFile* of = new OutputFile( p_words[3] );
            of->SetFlushPeriod( atoi( p_words[4].c_str() ) );
            m_outputs[p_words[2]] = of;
        }
        else
        {
            char comment[128];
            sprintf( comment, "log configuration file : unknown class for output, line %d", p_line_num );
            _DSEXCEPTION( comment );   
            return false;
        }
    }

    return true;
}

void Logger::Configuration::RegisterSink( Sink* p_sink )
{
    dsstring name;
    p_sink->GetName( name );

    if( m_sinks.count( name ) > 0 )
    {
        // entry exists

        m_sinks[name].sink = p_sink;
        p_sink->SetState( m_sinks[name].state );
        p_sink->SetCurrentLevel( m_sinks[name].level );
        p_sink->RegisterOutput( m_sinks[name].output );
    }
    else
    {
        // entry does not exists, create it

        SinkEntry sink_entry;
        sink_entry.sink = p_sink;
        m_sinks[name] = sink_entry;
    }
}