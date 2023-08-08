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

#include <cmath>

#include "timemanager.h"
using namespace DrawSpace;


void Timer::setState( bool p_state )
{
    m_state = p_state;
    m_prev_tick = -1;
    m_tick_count = 0;
    m_freeze = false;
}

void Timer::suspend( bool p_suspend )
{
    m_freeze = p_suspend;
    if( p_suspend )
    {
        m_prev_tick = -1;
    }
}

void Timer::setPeriod( long p_period )
{
    m_period = p_period;
}

void Timer::setHandler( TimerHandler* p_handler )
{
    m_handler = p_handler;
}

void Timer::expired( void )
{
    if( m_handler )
    {
        (*m_handler )( this );
    }
}

///////////////////////////////////////////////////////////////////////

TimeManager::TimeManager( void )
{
    reset();
}

void TimeManager::reset( void )
{
    m_last_tick         = 0;
	m_current_tick		= 0;
    m_fps               = 0;
    m_frame_count       = 0;
    m_ready             = false;
    m_last_deltatime    = 0;
}

void TimeManager::update( void )
{
    long current_tick = ::GetTickCount();

    if( m_last_tick )
    {
        if(current_tick - m_last_tick >= 1000 )
        {
            m_last_deltatime = current_tick - m_last_tick;
            m_last_tick = current_tick;
            m_fps = m_frame_count;
            m_frame_count = 0;

            if( m_fps > 1 )
            {
                // declare ready only if we get a decent fps
                m_ready = true;
            }
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
        for( auto it = m_timers.begin(); it != m_timers.end(); ++it )
        {
            Timer* timer = (*it);

            if( timer->m_state && !timer->m_freeze )
            {
                if( -1 == timer->m_prev_tick )
                {                
                    timer->m_prev_tick = current_tick;
                }
                else
                {
                    timer->m_tick_count += current_tick - timer->m_prev_tick;
                    if( timer->m_tick_count >= timer->m_period )
                    {
                        timer->m_tick_count = 0;                        
                        timer->expired();
                    }

                    timer->m_prev_tick = current_tick;
                }                
            }
        }
		m_current_tick = current_tick;
    }
}

long TimeManager::getFPS( void ) const
{
    return m_fps;
}

//vitesse en degres par seconde
void TimeManager::angleSpeedInc( dsreal *p_angle, dsreal p_angleSpeed )
{
    if( !m_ready ) return;
    
    // on veut, a partir de la vitesse en degres/s fixee, trouver
    // la vitesse en degres / frame -> on fait donc (deg/sec)/(frame/sec) 
    dsreal angleSpeedDegPerFrame = p_angleSpeed / m_fps; 
    dsreal angle = *p_angle;

    angle += angleSpeedDegPerFrame;
    angle = std::fmod( angle, 360.0 );

    *p_angle = angle;
}

void TimeManager::angleSpeedDec( dsreal *p_angle, dsreal p_angleSpeed )
{
    if( !m_ready ) return;

    // on veut, a partir de la vitesse en degres/s fixee, trouver
    // la vitesse en degres / frame -> on fait donc (deg/sec)/(frame/sec) 
    dsreal angleSpeedDegPerFrame = p_angleSpeed / m_fps;      
    dsreal angle = *p_angle;

    angle -= angleSpeedDegPerFrame;
    angle = std::fmod( angle, 360.0 );
       
   if( *p_angle <= 0.0f )
    {
        angle = 360.0 + angle;
    }

    *p_angle = angle;   
}

void TimeManager::translationSpeedInc( dsreal *p_translation, dsreal p_speed )
{
    if( !m_ready ) return;

    // on veut, a partir de la vitesse en unites/s fixee, trouver
    // la vitesse en unite / frame -> on fait donc (unit/sec)/(frame/sec)
    dsreal translationSpeedUnitPerFrame = p_speed / m_fps;
    *p_translation += translationSpeedUnitPerFrame;
}

void TimeManager::translationSpeedDec( dsreal *p_translation, dsreal p_speed )
{
    if( !m_ready ) return;

    // on veut, a partir de la vitesse en unites/s fixee, trouver
    // la vitesse en unite / frame -> on fait donc (unit/sec)/(frame/sec)
    dsreal translationSpeedUnitPerFrame = p_speed / m_fps;
    *p_translation -= translationSpeedUnitPerFrame;
}

dsreal TimeManager::convertUnitPerSecFramePerSec( dsreal p_speed )
{
    if( !m_ready ) 
    {
        return 0.0;
    }
    return ( p_speed / m_fps );
}

bool TimeManager::isReady( void ) const
{
    return m_ready;
}

long TimeManager::getLastDeltaTime( void ) const
{
    return m_last_deltatime;
}

long TimeManager::getCurrentTick(void) const
{
	return m_current_tick;
}

void TimeManager::registerTimer( Timer* p_timer )
{
    m_timers.insert( p_timer );
}

void TimeManager::unregisterTimer( Timer* p_timer )
{
    if( m_timers.count( p_timer ) )
    {
        m_timers.erase( p_timer );
    }
}
