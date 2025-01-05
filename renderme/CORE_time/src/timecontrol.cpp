
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

#include <time.h>
#include "timecontrol.h"
#include "datacloud.h"
#include "syncvariable.h"

using namespace renderMe;
using namespace renderMe::core;


TimeControl::TimeControl()
{
    const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };

    dataCloud->registerData<long>("std.framesPerSecond", 0);
    dataCloud->registerData<std::string>("std.date", "");

    dataCloud->registerData<std::string>("std.timeFactor", "");
    dataCloud->registerData<__time64_t>("std.currentTime");

}

// TODO :
//      add TimeMark class
//      integrate (set in time system, replace time manager, fix compils errors)
    
void TimeControl::update(void)
{
	m_tm.update();

    const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };

    // put current formated date/time in component string[0]
    
    std::string      datetime{ "?" };
    const struct tm* mytime{ _gmtime64(&m_current_time) };
    if (mytime)
    {
        datetime = asctime(mytime);
    }

    dataCloud->updateDataValue<std::string>("std.date", datetime);
    dataCloud->updateDataValue<__time64_t>("std.currentTime", m_current_time);
    dataCloud->updateDataValue<long>("std.framesPerSecond", m_tm.getFPS());
    dataCloud->updateDataValue<std::string>("std.timeFactor", m_mode_str);
}

bool TimeControl::isReady(void) const
{
	return m_tm.isReady();
}

void TimeControl::angleSpeedInc(double* p_angle, double p_angleSpeed)
{
    if (m_tm.isReady())
    {
        m_tm.angleSpeedInc(p_angle, p_angleSpeed * m_time_factor);
    }
}

void TimeControl::angleSpeedDec(double* p_angle, double p_angleSpeed)
{
    if (m_tm.isReady())
    {
        m_tm.angleSpeedDec(p_angle, p_angleSpeed * m_time_factor);
    }
}

void TimeControl::translationSpeedInc(double* p_translation, double p_speed)
{
    if (m_tm.isReady())
    {
        m_tm.translationSpeedInc(p_translation, p_speed * m_time_factor);
    }
}

void TimeControl::translationSpeedDec(double* p_translation, double p_speed)
{
    if (m_tm.isReady())
    {
        m_tm.translationSpeedDec(p_translation, p_speed * m_time_factor);
    }
}


void TimeControl::manageVariable(SyncVariable& p_variable)
{
    if (p_variable.state == SyncVariable::State::OFF)
    {
        return;
    }

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
        else if (SyncVariable::Direction::DEC == p_variable.direction)
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

double TimeControl::convertUnitPerSecFramePerSec(double p_speed)
{
	return m_tm.convertUnitPerSecFramePerSec(p_speed) * m_time_factor;
}

void TimeControl::setTimeFactor(TimeControl::TimeScale p_scale)
{
    switch (p_scale)
    {
        case TimeScale::NORMAL_TIME:

            m_mode_str = "X1";

            m_time_period = 1000;
            m_time_factor = 1;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::MUL2_TIME:

            m_mode_str = "X2";

            m_time_period = 500;
            m_time_factor = 2;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 2;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::MUL4_TIME:

            m_mode_str = "X4";

            m_time_period = 250;
            m_time_factor = 4;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 4;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::MUL10_TIME:

            m_mode_str = "X10";

            m_time_period = 100;
            m_time_factor = 10;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::MUL100_TIME:

            m_mode_str = "X100";

            m_time_period = 100;
            m_time_factor = 100.0;
            m_current_time_increment = 10;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::MUL500_TIME:

            m_mode_str = "X500";

            m_time_period = 100;
            m_time_factor = 500.0;
            m_current_time_increment = 50;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::SEC_1HOUR_TIME:

            m_mode_str = "1SEC/HOUR";

            m_time_period = 100;
            m_time_factor = 3600.0;
            m_current_time_increment = 360;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::SEC_1DAY_TIME:

            m_mode_str = "1SEC/DAY";

            m_time_period = 100;
            m_time_factor = 86400.0;
            m_current_time_increment = 8640;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::SEC_30DAYS_TIME:

            m_mode_str = "1SEC/30D";

            m_time_period = 100;
            m_time_factor = 2592000.0;
            m_current_time_increment = 259200;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = 200;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::SEC_1YEAR_TIME:

            m_mode_str = "1SEC/1Y";

            m_time_period = 100;
            m_time_factor = 86400.0 * 365.0;
            m_current_time_increment = 86400 * 365;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::DIV2_TIME:

            m_mode_str = "/2";

            m_time_period = 1000;
            m_time_factor = 0.5;
            m_sub_sec_count_lim = 2;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = m_base_timestep;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::DIV4_TIME:

            m_mode_str = "/4";

            m_time_period = 1000;
            m_time_factor = 0.25;
            m_sub_sec_count_lim = 4;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = m_base_timestep;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::DIV10_TIME:

            m_mode_str = "/10";

            m_time_period = 1000;
            m_time_factor = 0.1;
            m_sub_sec_count_lim = 10;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = m_base_timestep;

            m_freeze = false;
            //m_timer.suspend(false);
            break;

        case TimeScale::FREEZE:

            m_mode_str = "FREEZE";

            m_freeze = true;
            //m_timer.suspend(true);
            break;
    }

    m_mode = p_scale;

    /*
    if (m_active && !m_freeze)
    {
        m_timer.setState(false);
        m_timer.setPeriod(m_time_period);
        m_timer.setState(true);
    }
    */
}
