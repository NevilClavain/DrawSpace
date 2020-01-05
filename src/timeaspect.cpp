/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#include <time.h>
#include "timeaspect.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;

TimeAspect::TimeAspect( void ) : 
m_time_factor( 1.0 ),
m_tm( NULL ),
m_time_period( 1000 ),
m_current_time_increment( 1 ),
m_sub_sec_count( 0 ),
m_sub_sec_count_lim( 0 ),
m_mode( NORMAL_TIME ),
m_active( false ),
m_world_nbsteps( 5 ),
m_freeze( false )
{
    m_timercb = _DRAWSPACE_NEW_( TimerCb, TimerCb( this, &TimeAspect::on_timer ) );

    m_timer.SetHandler( m_timercb );
    m_timer.SetPeriod( m_time_period );
}

TimeAspect::~TimeAspect( void )
{
    _DRAWSPACE_DELETE_( m_timercb );
}

TimeAspect::TimeAngle TimeAspect::TimeAngleFactory( dsreal p_initvalue )
{
    get_tm();
    return TimeAngle( p_initvalue, m_tm, &m_time_factor, &m_freeze );
}


TimeAspect::TimeScalar TimeAspect::TimeScalarFactory( dsreal p_initvalue )
{
    get_tm();
    return TimeScalar( p_initvalue, m_tm, &m_time_factor, &m_freeze );
}

TimeAspect::TimeMark TimeAspect::TimeMarkFactory(void)
{
	get_tm();
	return TimeMark(m_tm, &m_time_factor, &m_freeze);
}

dsreal TimeAspect::ConvertUnitPerSecFramePerSec( dsreal p_speed )
{
    get_tm();
    return m_tm->ConvertUnitPerSecFramePerSec( p_speed ) * m_time_factor;
}

void TimeAspect::get_tm( void )
{
    if( m_tm )
    {
        return;
    }

    ComponentList<TimeManager> tms;
    GetComponentsByType<TimeManager>( tms );

    TimeManager* tm = NULL;
    if( tms.size() > 0 )
    {
        tm = &tms[0]->getPurpose();
    }
    else
    {
         _DSEXCEPTION( "No time manager associated with TimeAspect!!!" )
    }

    m_tm = tm;
    m_tm->RegisterTimer( &m_timer );
}

void TimeAspect::Update( void )
{
    get_tm();

    if( m_active )
    {
        m_tm->Update();

        ComponentList<dsstring> strs;
        GetComponentsByType<dsstring>( strs );

        // put current formated date/time in component string[0]
        struct tm* mytime;

        mytime = _gmtime64( &m_current_time );
        if( mytime )
        {
            strs[0]->getPurpose() = asctime( mytime );
        }
        else
        {
            strs[0]->getPurpose() = "???";
        }

        ComponentList<dstime> times_count;
        GetComponentsByType<dstime>( times_count );
        // update current time
        times_count[0]->getPurpose() = m_current_time;

        ComponentList<int> ints;
        GetComponentsByType<int>( ints );

        // put current fps

        if( m_tm->IsReady() )
        {
            ints[0]->getPurpose() = m_tm->GetFPS();
        }
        else
        {
            ints[0]->getPurpose() = 0;
        }


        // update physic world nb steps

        ints[1]->getPurpose() = m_world_nbsteps;

        // update time factor

        ComponentList<dsreal> reals;
        GetComponentsByType<dsreal>( reals );

        reals[0]->getPurpose() = m_time_factor;

        // gestion scale (ou mode) temp

        ComponentList<TimeScale> time_scales;
        GetComponentsByType<TimeScale>( time_scales );

        if( m_mode != time_scales[0]->getPurpose() )
        {
            // update !
            set_time_factor( time_scales[0]->getPurpose() );
        }
    }
}

void TimeAspect::Activate( void )
{
    m_tm->Reset();
    ComponentList<dstime> times_count;
    GetComponentsByType<dstime>( times_count );

    if( times_count.size() > 0 )
    {
        m_current_time = times_count[0]->getPurpose();
    }
    
    m_timer.SetPeriod( m_time_period );
    m_timer.SetState( true );

    m_active = true;
}

void TimeAspect::Deactivate( void )
{
    m_timer.SetState( false );
    m_active = false;
    m_tm->Reset();
}

void TimeAspect::on_timer( DrawSpace::Utils::Timer* p_timer )
{
    if( 0 == m_sub_sec_count_lim )
    {
        m_current_time += m_current_time_increment;
    }
    else
    {
        m_sub_sec_count++;
        if( m_sub_sec_count == m_sub_sec_count_lim )
        {
            m_sub_sec_count = 0;
            m_current_time++;
        }
    }
}

void TimeAspect::set_time_factor( TimeAspect::TimeScale p_scale )
{
    switch( p_scale )
    {
        case NORMAL_TIME:

            m_time_period = 1000;
            m_time_factor = 1;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep;

            m_freeze = false;
            m_timer.Suspend( false );

            break;

        case MUL2_TIME:

            m_time_period = 500;
            m_time_factor = 2;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 2;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case MUL4_TIME:
           
            m_time_period = 250;
            m_time_factor = 4;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 4;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case MUL10_TIME:

            m_time_period = 100;
            m_time_factor = 10;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case MUL100_TIME:

            m_time_period = 100;
            m_time_factor = 100.0;
            m_current_time_increment = 10;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case MUL500_TIME:

            m_time_period = 100;
            m_time_factor = 500.0;
            m_current_time_increment = 50;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            m_timer.Suspend( false );


            break;


        case SEC_1HOUR_TIME:

            m_time_period = 100;
            m_time_factor = 3600.0;
            m_current_time_increment = 360;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case SEC_1DAY_TIME:

            m_time_period = 100;
            m_time_factor = 86400.0;
            m_current_time_increment = 8640;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case SEC_30DAYS_TIME:

            m_time_period = 100;
            m_time_factor = 2592000.0;
            m_current_time_increment = 259200;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = 200;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case SEC_1YEAR_TIME:

            m_time_period = 100;
            m_time_factor = 86400.0 * 365.0;
            m_current_time_increment = 86400 * 365;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = m_base_timestep * 10;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case DIV2_TIME:

            m_time_period = 1000;
            m_time_factor = 0.5;
            m_sub_sec_count_lim = 2;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = m_base_timestep;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case DIV4_TIME:

            m_time_period = 1000;
            m_time_factor = 0.25;
            m_sub_sec_count_lim = 4;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = m_base_timestep;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case DIV10_TIME:

            m_time_period = 1000;
            m_time_factor = 0.1;
            m_sub_sec_count_lim = 10;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = m_base_timestep;

            m_freeze = false;
            m_timer.Suspend( false );


            break;

        case FREEZE:

            m_freeze = true;
            m_timer.Suspend( true );
            break;
    }

    m_mode = p_scale;

    if( m_active && !m_freeze )
    {
        m_timer.SetState( false );
        m_timer.SetPeriod( m_time_period );
        m_timer.SetState( true );
    }
}


void TimeAspect::TimeMark::Reset(void)
{
	m_previous_tick = 0;
}

long TimeAspect::TimeMark::ComputeTimeMs(void)
{
	long ms_result = 0;

	if (m_tm->IsReady())
	{
		long last_tick = m_tm->GetCurrentTick();

		if (m_previous_tick != 0)
		{
			if (false == *m_freeze)
			{
				m_timecounter += (*m_timefactor) * (last_tick - m_previous_tick);
			}			
			ms_result = m_timecounter;		
		}
		m_previous_tick = last_tick;
	}
	return ms_result;
}
