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

#pragma once

namespace renderMe
{
	namespace core
	{
        class TimeManager
        {
        public:
            TimeManager(void);
            ~TimeManager(void) = default;

            struct Variable
            {
            public:
                enum class Type
                {
                    ANGLE,
                    POSITION
                };

                Variable() = default;
                ~Variable() = default;

                Variable(Type p_type, double p_step, bool p_increment = true, double p_initial_value = 0.0) :
                type(p_type),
                step(p_step),
                increment(p_increment),
                value(p_initial_value)
                {
                };

                double  value{ 0.0 };
                double  step{ 0.0 };
                bool    increment{ true };
                Type    type;                
            };

            void    reset(void);

            void    update(void);

            void    manageVariable(Variable& p_variable);

            void    angleSpeedInc(double* p_angle, double p_angleSpeed);
            void    angleSpeedDec(double* p_angle, double p_angleSpeed);
            void    translationSpeedInc(double* p_translation, double p_speed);
            void    translationSpeedDec(double* p_translation, double p_speed);

            double  convertUnitPerSecFramePerSec(double p_speed);

            bool    isReady(void) const;
            long    getLastDeltaTime(void) const;
            long    getCurrentTick(void) const;
            long    getFPS(void) const;

        private:
            long    m_last_tick{ 0 };
            long    m_frame_count{ 0 };
            long    m_fps{ 0 };
            bool    m_ready{ false };
            long    m_last_deltatime{ 0 };
            long    m_current_tick{ 0 };

        };

	} // core
} //renderMe
