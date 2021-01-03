/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#ifndef _TIMEMANAGER_H_
#define _TIMEMANAGER_H_

#include "drawspace_commons.h"
#include "callback.h"

namespace DrawSpace
{
namespace Utils
{
class Timer
{
public:
    typedef Core::BaseCallback<void, Timer*> TimerHandler;

protected:

    bool            m_state;
    long            m_period; // ms
    TimerHandler*   m_handler;
    long            m_tick_count;
    long            m_prev_tick;
    bool            m_freeze;

    void expired( void );

public:

    Timer( void );
    ~Timer( void );

    void    SetState( bool p_state );
    void    Suspend( bool p_suspend );
    void    SetPeriod( long p_period );
    void    SetHandler( TimerHandler* p_handler );

    friend class TimeManager;
};

class TimeManager
{
protected:

    long    m_last_tick;	
    long    m_frame_count;
    long    m_fps;
    bool    m_ready;
    long    m_last_deltatime;
	long    m_current_tick;

    using TimerHandler = Core::BaseCallback<void, const dsstring&>;

    std::set<Timer*>    m_timers;

public:
    TimeManager( void ); 
    ~TimeManager( void );

    void    Reset( void );

    void    Update( void );

    //vitesse en degres par seconde
    void    AngleSpeedInc( dsreal *p_angle, dsreal p_angleSpeed );
    void    AngleSpeedDec( dsreal *p_angle, dsreal p_angleSpeed );

    void    TranslationSpeedInc( dsreal *p_translation, dsreal p_speed );
    void    TranslationSpeedDec( dsreal *p_translation, dsreal p_speed );

    dsreal  ConvertUnitPerSecFramePerSec( dsreal p_speed );

    bool    IsReady( void ) const ;
    long    GetLastDeltaTime( void ) const ;
	long    GetCurrentTick(void) const;
	long    GetFPS(void) const;

    void    RegisterTimer( Timer* p_timer );
    void    UnregisterTimer( Timer* p_timer );

};
}
}

#endif
