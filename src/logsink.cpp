/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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



#include "logsink.h"
#include "logconf.h"

using namespace DrawSpace;


Logger::Sink::Sink( const dsstring& p_name, Logger::Configuration* p_conf ) : 
m_name( p_name ),
m_current_level( LEVEL_WARN ),
m_state( false ),
m_output( NULL ),
m_conf( p_conf )
{
    if( p_conf )
    {
        p_conf->RegisterSink( this );
    }
}

Logger::Sink::~Sink( void )
{
}

void Logger::Sink::SetCurrentLevel( Logger::Sink::Level p_level )
{
    m_current_level = p_level;
}

void Logger::Sink::SetState( bool p_state )
{
    m_state = p_state;
}

void Logger::Sink::LogIt( Level p_level, const dsstring& p_trace )
{
    if( p_level <= m_current_level && m_state && m_output )
    {
        dsstring level;

        if( p_level == LEVEL_FATAL )
        {
            level = "FATAL";
        }
        else if( p_level == LEVEL_ERROR )
        {
            level = "ERROR";
        }
        else if( p_level == LEVEL_WARN )
        {
            level = "WARN";
        }
        else if( p_level == LEVEL_DEBUG )
        {
            level = "DEBUG";
        }
        else if( p_level == LEVEL_TRACE )
        {
            level = "TRACE";
        }

        char thread_id[16];
        sprintf( thread_id, "[%.8x]", GetCurrentThreadId() );

        char timestamp[32];

        if( m_conf )
        {
            double timestamp_in_second = m_conf->GetLastTick() / 1000000.0;
            sprintf(timestamp, "%.6f", timestamp_in_second);
        }
        else
        {
            sprintf( timestamp, "??????????" );
        }

        dsstring final_trace = dsstring( timestamp ) + dsstring( " " ) + dsstring( thread_id ) + dsstring( " " ) + m_name + dsstring( " " ) + level + dsstring( " " ) + dsstring( "[ " ) + p_trace + dsstring( " ]" ) + dsstring( "\n" );
        m_output->LogIt( final_trace );
    }

    if( m_conf )
    {
        m_conf->UpdateTick();
    }
}

void Logger::Sink::RegisterOutput( Logger::Output* p_output )
{
    m_output = p_output;
}

void Logger::Sink::GetName( dsstring& p_name ) const
{
    p_name = m_name;
}

void Logger::Sink::SetConfiguration( Logger::Configuration* p_conf )
{
    m_conf = p_conf;
}
