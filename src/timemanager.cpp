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

#include "timemanager.h"
using namespace DrawSpace::Utils;

TimeManager::TimeManager( void )
{
    Reset();
}

TimeManager::~TimeManager( void )
{

}

void TimeManager::Reset( void )
{
    m_last_tick         = 0;
    m_fps               = 0;
    m_frame_count       = 0;
    m_ready             = false;
    m_last_deltatime    = 0;
}

void TimeManager::Update( void )
{
    long current_tick;
    current_tick = GetTickCount();

    if( m_last_tick )
    {
        if( current_tick - m_last_tick >= 1000 )
        {
            m_last_deltatime = current_tick - m_last_tick;
            m_ready = true;
            m_last_tick = current_tick;
            m_fps = m_frame_count;
            m_frame_count = 0;
        }		
    }
    else
    {
        m_last_tick = current_tick;
    }
    m_frame_count++;

    if( m_ready )
    {
        // timers management
        for( std::map<dsstring, timer_entry>::iterator it = m_timers.begin(); it != m_timers.end(); ++it )
        {
            timer_entry current = (*it).second;

            if( current.state && !current.freeze )
            {
                if( -1 == current.prev_tick )
                {                
                    current.prev_tick = current_tick;
                }
                else
                {
                    current.tick_count += current_tick - current.prev_tick;

                    if( current.tick_count >= current.period )
                    {
                        current.tick_count = 0;                        
                        ( *current.handler )( (*it).first );
                    }

                    current.prev_tick = current_tick;
                }

                (*it).second = current;
            }
        }
    }
}

long TimeManager::GetFPS( void )
{
    return m_fps;
}

//vitesse en degres par seconde
void TimeManager::AngleSpeedInc( dsreal *p_angle, dsreal p_angleSpeed )
{
    if( !m_ready ) return;
    
    // on veut, a partir de la vitesse en degres/s fixee, trouver
    // la vitesse en degres / frame -> on fait donc (deg/sec)/(frame/sec) 
    dsreal angleSpeedDegPerFrame = p_angleSpeed / m_fps;

    *p_angle += angleSpeedDegPerFrame;
    if( *p_angle >= 360.0 )
    {
        *p_angle -= 360.0;
    }
}

void TimeManager::AngleSpeedDec( dsreal *p_angle, dsreal p_angleSpeed )
{
    if( !m_ready ) return;

    // on veut, a partir de la vitesse en degres/s fixee, trouver
    // la vitesse en degres / frame -> on fait donc (deg/sec)/(frame/sec) 
    dsreal angleSpeedDegPerFrame = p_angleSpeed / m_fps;

    *p_angle -= angleSpeedDegPerFrame;
    if( *p_angle <= 0.0f )
    {
        *p_angle = 360.0 + *p_angle;
    }
}

void TimeManager::TranslationSpeedInc( dsreal *p_translation, dsreal p_speed )
{
    if( !m_ready ) return;

    // on veut, a partir de la vitesse en unites/s fixee, trouver
    // la vitesse en unite / frame -> on fait donc (unit/sec)/(frame/sec)
    dsreal translationSpeedUnitPerFrame = p_speed / m_fps;
    *p_translation += translationSpeedUnitPerFrame;
}

void TimeManager::TranslationSpeedDec( dsreal *p_translation, dsreal p_speed )
{
    if( !m_ready ) return;

    // on veut, a partir de la vitesse en unites/s fixee, trouver
    // la vitesse en unite / frame -> on fait donc (unit/sec)/(frame/sec)
    dsreal translationSpeedUnitPerFrame = p_speed / m_fps;
    *p_translation -= translationSpeedUnitPerFrame;
}

dsreal TimeManager::ConvertUnitPerSecFramePerSec( dsreal p_speed )
{
    if( !m_ready ) 
    {
        return 0.0;
    }
    return ( p_speed / m_fps );
}

bool TimeManager::IsReady( void )
{
    return m_ready;
}

long TimeManager::GetLastDeltaTime( void )
{
    return m_last_deltatime;
}

void TimeManager::AddTimer( const dsstring& p_id, long p_period, TimerHandler* p_handler )
{
    timer_entry t_entry;

    t_entry.state = false;
    t_entry.period = p_period;
    t_entry.handler = p_handler;
    t_entry.freeze = false;

    m_timers[p_id] = t_entry;
}

void TimeManager::SetTimerState( const dsstring& p_id, bool p_state )
{
    if( 0 == m_timers.count( p_id ) )
    {
        return;
    }
    m_timers[p_id].state = p_state;
    m_timers[p_id].prev_tick = -1;
    m_timers[p_id].tick_count = 0;
    m_timers[p_id].freeze = false;

}

void TimeManager::SetTimerPeriod( const dsstring& p_id, long p_period )
{
    if( 0 == m_timers.count( p_id ) )
    {
        return;
    }
    m_timers[p_id].period = p_period;
}

void TimeManager::SuspendTimer( const dsstring& p_id, bool p_suspend )
{
    if( 0 == m_timers.count( p_id ) )
    {
        return;
    }
    m_timers[p_id].freeze = p_suspend;

    if( p_suspend )
    {
        m_timers[p_id].prev_tick = -1;
    }
}

void TimeManager::ClearAllTimers( void )
{
    m_timers.clear();
}
