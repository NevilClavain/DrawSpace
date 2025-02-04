/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <set>
#include "eventsource.h"

namespace mage
{
	namespace core
	{
        enum class TimerEvents
        {
            TIMER_EXPIRED
        };

        class TimerDescr : public property::EventSource<TimerEvents>
        {
        public:
            TimerDescr() = default;
            ~TimerDescr() = default;

            void    setState(bool p_state);
            void    suspend(bool p_suspend);
            void    setPeriod(long p_period);

        private:

            bool            m_state{ false };
            long            m_period{ 1000 }; // ms
            long            m_tick_count{ 0 };
            long            m_prev_tick{ -1 };
            bool            m_freeze{ false };

            void            expired(void);

            friend class TimeManager;
        };

        class TimeManager
        {
        public:
            TimeManager() = default;
            ~TimeManager() = default;

            void    reset();

            void    update();

            void    angleSpeedInc(double* p_angle, double p_angleSpeed);
            void    angleSpeedDec(double* p_angle, double p_angleSpeed);
            void    translationSpeedInc(double* p_translation, double p_speed);
            void    translationSpeedDec(double* p_translation, double p_speed);

            double  convertUnitPerSecFramePerSec(double p_speed);

            bool    isReady() const;
            long    getLastDeltaTime() const;
            long    getCurrentTick() const;
            long    getFPS() const;

            void    registerTimer(TimerDescr* p_timer);

        private:
            long                        m_last_tick{ 0 };
            long                        m_frame_count{ 0 };
            long                        m_fps{ 0 };
            bool                        m_ready{ false };
            long                        m_last_deltatime{ 0 };
            long                        m_current_tick{ 0 };

            std::set<TimerDescr*>        m_timers;

        };

	} // core
} //mage
