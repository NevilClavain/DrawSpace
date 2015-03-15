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


#include "logsink.h"
#include "logconf.h"

using namespace DrawSpace;


Logger::Sink::Sink( const dsstring& p_name ) : 
m_name( p_name ),
m_current_level( LEVEL_WARN ),
m_state( false ),
m_output( NULL )
{
    Logger::Configuration::GetInstance()->RegisterSink( this );
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
        dsstring final_trace = m_name + p_trace + dsstring( "\n" );
        m_output->LogIt( final_trace );
    }
}

void Logger::Sink::RegisterOutput( Logger::Output* p_output )
{
    m_output = p_output;
}

void Logger::Sink::GetName( dsstring& p_name )
{
    p_name = m_name;
}