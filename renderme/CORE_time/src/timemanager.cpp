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

#define _USE_MATH_DEFINES

#include <windows.h>
#include <cmath>
#include <numbers>

#include "timemanager.h"
#include "syncvariable.h"



using namespace renderMe::core;

TimeManager::TimeManager()
{
}

void TimeManager::reset(void)
{
    m_last_tick = 0;
    m_current_tick = 0;
    m_fps = 0;
    m_frame_count = 0;
    m_ready = false;
    m_last_deltatime = 0;
}

void TimeManager::update(void)
{
    const auto current_tick{ ::GetTickCount() };

    if (m_last_tick)
    {
        if (current_tick - m_last_tick >= 1000)
        {
            m_last_deltatime = current_tick - m_last_tick;
            m_last_tick = current_tick;
            m_fps = m_frame_count;
            m_frame_count = 0;

            if (m_fps > 1)
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

    if (m_ready)
    {
        // TODO later
        /*
        // timers management
        for (auto it = m_timers.begin(); it != m_timers.end(); ++it)
        {
            const auto timer{ (*it) };

            if (timer->m_state && !timer->m_freeze)
            {
                if (-1 == timer->m_prev_tick)
                {
                    timer->m_prev_tick = current_tick;
                }
                else
                {
                    timer->m_tick_count += current_tick - timer->m_prev_tick;
                    if (timer->m_tick_count >= timer->m_period)
                    {
                        timer->m_tick_count = 0;
                        timer->expired();
                    }

                    timer->m_prev_tick = current_tick;
                }
            }
        }
        */
        m_current_tick = current_tick;
    }
}


bool TimeManager::isReady(void) const
{
    return m_ready;
}

long TimeManager::getLastDeltaTime(void) const
{
    return m_last_deltatime;
}

long TimeManager::getCurrentTick(void) const
{
    return m_current_tick;
}

long TimeManager::getFPS(void) const
{
    return m_fps;
}

double TimeManager::convertUnitPerSecFramePerSec(double p_speed)
{
    if (!m_ready)
    {
        return 0.0;
    }
    return (p_speed / m_fps);
}

void TimeManager::angleSpeedInc(double* p_angle, double p_angleSpeed)
{
    if (!m_ready) return;

    // on veut, a partir de la vitesse en degres/s fixee, trouver
    // la vitesse en degres / frame -> on fait donc (deg/sec)/(frame/sec) 
    const double angleSpeedDegPerFrame{ p_angleSpeed / m_fps };
    double angle{ *p_angle };

    angle += angleSpeedDegPerFrame;
    angle = std::fmod(angle, 2 * M_PI);

    *p_angle = angle;
}

void TimeManager::angleSpeedDec(double* p_angle, double p_angleSpeed)
{
    if (!m_ready) return;

    // on veut, a partir de la vitesse en degres/s fixee, trouver
    // la vitesse en degres / frame -> on fait donc (deg/sec)/(frame/sec) 
    const double angleSpeedDegPerFrame{ p_angleSpeed / m_fps };
    double angle{ *p_angle };

    angle -= angleSpeedDegPerFrame;
    angle = std::fmod(angle, 2 * M_PI);

    if (*p_angle <= 0.0f)
    {
        angle = 2 * M_PI + angle;
    }

    *p_angle = angle;
}

void TimeManager::translationSpeedInc(double* p_translation, double p_speed)
{
    if (!m_ready) return;

    // on veut, a partir de la vitesse en unites/s fixee, trouver
    // la vitesse en unite / frame -> on fait donc (unit/sec)/(frame/sec)
    const double translationSpeedUnitPerFrame{ p_speed / m_fps };
    *p_translation += translationSpeedUnitPerFrame;
}

void TimeManager::translationSpeedDec(double* p_translation, double p_speed)
{
    if (!m_ready) return;

    // on veut, a partir de la vitesse en unites/s fixee, trouver
    // la vitesse en unite / frame -> on fait donc (unit/sec)/(frame/sec)
    const double translationSpeedUnitPerFrame{ p_speed / m_fps };
    *p_translation -= translationSpeedUnitPerFrame;
}

void TimeManager::manageVariable(SyncVariable& p_variable)
{
    if (SyncVariable::Type::ANGLE == p_variable.type)
    {
        if (SyncVariable::Direction::INC == p_variable.direction)
        {
            angleSpeedInc(&p_variable.value, p_variable.step);

            if (p_variable.boundaries.max != NAN)
            {
                if (p_variable.value > p_variable.boundaries.max)
                {
                    switch (p_variable.boundaries_management)
                    {
                        case SyncVariable::BoundariesManagement::STOP:

                            p_variable.value = p_variable.boundaries.max;
                            p_variable.direction = SyncVariable::Direction::ZERO;
                            break;

                        case SyncVariable::BoundariesManagement::MIRROR:

                            p_variable.value = p_variable.boundaries.max;
                            p_variable.direction = SyncVariable::Direction::DEC;
                            break;

                        case SyncVariable::BoundariesManagement::WRAP:

                            p_variable.value = p_variable.boundaries.min;
                            break;
                    }
                }
            }
        }
        else if(SyncVariable::Direction::DEC == p_variable.direction)
        {
            angleSpeedDec(&p_variable.value, p_variable.step);

            if (p_variable.boundaries.min != NAN)
            {
                if (p_variable.value < p_variable.boundaries.min)
                {
                    switch (p_variable.boundaries_management)
                    {
                        case SyncVariable::BoundariesManagement::STOP:

                            p_variable.value = p_variable.boundaries.min;
                            p_variable.direction = SyncVariable::Direction::ZERO;
                            break;

                        case SyncVariable::BoundariesManagement::MIRROR:

                            p_variable.value = p_variable.boundaries.min;
                            p_variable.direction = SyncVariable::Direction::INC;
                            break;

                        case SyncVariable::BoundariesManagement::WRAP:

                            p_variable.value = p_variable.boundaries.max;
                            break;
                    }
                }
            }
        }
    }
    else if (SyncVariable::Type::POSITION == p_variable.type)
    {
        if (SyncVariable::Direction::INC == p_variable.direction)
        {
            translationSpeedInc(&p_variable.value, p_variable.step);

            if (p_variable.boundaries.max != NAN)
            {
                if (p_variable.value > p_variable.boundaries.max)
                {                   
                    switch (p_variable.boundaries_management)
                    {
                        case SyncVariable::BoundariesManagement::STOP:

                            p_variable.value = p_variable.boundaries.max;
                            p_variable.direction = SyncVariable::Direction::ZERO;
                            break;

                        case SyncVariable::BoundariesManagement::MIRROR:

                            p_variable.value = p_variable.boundaries.max;
                            p_variable.direction = SyncVariable::Direction::DEC;
                            break;

                        case SyncVariable::BoundariesManagement::WRAP:

                            p_variable.value = p_variable.boundaries.min;
                            break;
                    }
                }
            }
        }
        else if (SyncVariable::Direction::DEC == p_variable.direction)
        {
            translationSpeedDec(&p_variable.value, p_variable.step);

            if (p_variable.boundaries.min != NAN)
            {
                if (p_variable.value < p_variable.boundaries.min)
                {
                    switch (p_variable.boundaries_management)
                    {
                        case SyncVariable::BoundariesManagement::STOP:

                            p_variable.value = p_variable.boundaries.min;
                            p_variable.direction = SyncVariable::Direction::ZERO;
                            break;

                        case SyncVariable::BoundariesManagement::MIRROR:

                            p_variable.value = p_variable.boundaries.min;
                            p_variable.direction = SyncVariable::Direction::INC;
                            break;

                        case SyncVariable::BoundariesManagement::WRAP:

                            p_variable.value = p_variable.boundaries.max;
                            break;
                    }
                }
            }
        }
    }
}