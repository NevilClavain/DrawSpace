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

#pragma once

#include <set>
#include "ds_types.h"
#include "callback.h"

namespace DrawSpace
{
    class Timer
    {
    public:
        using TimerHandler = Core::BaseCallback<void, Timer*>;

        Timer( void ) = default;
        ~Timer( void ) = default;

        void    setState( bool p_state );
        void    suspend( bool p_suspend );
        void    setPeriod( long p_period );
        void    setHandler( TimerHandler* p_handler );

    private:

        bool            m_state         { false };
        long            m_period        { 1000 }; // ms
        TimerHandler*   m_handler       { nullptr };
        long            m_tick_count;
        long            m_prev_tick;
        bool            m_freeze        { false };

        void            expired(void);

        friend class TimeManager;
    };

    class TimeManager
    {
    public:
        TimeManager( void ); 
        ~TimeManager( void ) = default;

        void    reset( void );

        void    update( void );

        //vitesse en degres par seconde
        void    angleSpeedInc( dsreal *p_angle, dsreal p_angleSpeed );
        void    angleSpeedDec( dsreal *p_angle, dsreal p_angleSpeed );

        void    translationSpeedInc( dsreal *p_translation, dsreal p_speed );
        void    translationSpeedDec( dsreal *p_translation, dsreal p_speed );

        dsreal  convertUnitPerSecFramePerSec( dsreal p_speed );

        bool    isReady( void ) const ;
        long    getLastDeltaTime( void ) const ;
	    long    getCurrentTick(void) const;
	    long    getFPS(void) const;

        void    registerTimer( Timer* p_timer );
        void    unregisterTimer( Timer* p_timer );

    private:

        using TimerHandler = Core::BaseCallback<void, const dsstring&>;

        long    m_last_tick;
        long    m_frame_count;
        long    m_fps;
        bool    m_ready             { false };
        long    m_last_deltatime;
        long    m_current_tick;
   
        std::set<Timer*>    m_timers;

    };
}

