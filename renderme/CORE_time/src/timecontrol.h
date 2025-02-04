
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

#include <string>
#include "singleton.h"
#include "timemanager.h"

namespace mage
{
    namespace core
    {
        struct SyncVariable;

        // fwd decl
        class TimeControl;

        class TimeMark
        {       
        public:            
            TimeMark() = default;
            TimeMark(TimeControl& p_tc);
            ~TimeMark() = default;

            void reset(void);
            long computeTimeMs(void);

        private:
           
            TimeManager*    m_tm            { nullptr };
            double*         m_timefactor    { nullptr };
            bool*           m_freeze        { nullptr };

            long            m_previous_tick { 0 };
            long		    m_timecounter   { 0 };
        };

        class TimeControl : public property::Singleton<TimeControl>
        {
        public:

            enum class TimeScale
            {
                NORMAL_TIME,
                MUL2_TIME,
                MUL4_TIME,
                MUL10_TIME,
                MUL100_TIME,
                MUL500_TIME,
                SEC_1HOUR_TIME, // 1 sec = 1 hour -> x 3600        
                SEC_1DAY_TIME, // 1 sec = 24 hour -> x 86400
                SEC_30DAYS_TIME, // 1 sec = 30 days -> x 2592000
                SEC_1YEAR_TIME, // 1sec = 1 year -> x 31536000
                DIV2_TIME,
                DIV4_TIME,
                DIV10_TIME,
                FREEZE,
            };

            TimeControl();
            ~TimeControl() = default;

            void    update();
            bool    isReady() const;


            double  convertUnitPerSecFramePerSec(double p_speed);

            void    manageVariable(SyncVariable& p_variable);

            void    setTimeFactor(TimeControl::TimeScale p_scale);

            TimeControl::TimeScale getTimeFactor() const;

            void    angleSpeedInc(double* p_angle, double p_angleSpeed);
            void    angleSpeedDec(double* p_angle, double p_angleSpeed);
            void    translationSpeedInc(double* p_translation, double p_speed);
            void    translationSpeedDec(double* p_translation, double p_speed);

            TimeMark buildTimeMark();

        private:

            static const int        m_base_timestep             { 8 };

            TimeScale               m_mode                      { TimeScale::NORMAL_TIME };
            std::string             m_mode_str                  { "X1" };

            double                  m_time_factor               { 1.0 };  // calcul...

            long                    m_time_period               { 1000 };

            __time64_t              m_current_time              { 0 };
            long                    m_current_time_increment    { 1 };

            long                    m_sub_sec_count             { 0 };
            long                    m_sub_sec_count_lim         { 0 };

            bool                    m_freeze                    { false };

            int                     m_world_nbsteps             { m_base_timestep };

            TimeManager             m_tm;

            TimerDescr              m_timer;

            friend class TimeMark;

        };
    }
}
