/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "calendar.h"
#include <time.h>

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


Calendar::Calendar( dstime p_offset_time, DrawSpace::Utils::TimeManager* p_tm, DrawSpace::Dynamics::World* p_world ) : 
m_offset_time( p_offset_time ), 
m_time_mode( NORMAL_TIME ),
m_time_period( 1000 ),
m_time_factor( 1 ),
m_current_time_increment( 1 ),
m_time_manager( p_tm ),
m_active( false ),
m_world( p_world ),
m_sub_sec_count( 0 ),
m_sub_sec_count_lim( 0 ),
m_freeze( false )
{
    m_current_time = m_offset_time;
    m_timercb = _DRAWSPACE_NEW_( CalendarTimer, CalendarTimer( this, &Calendar::on_timer ) );

    p_tm->AddTimer( "calendar_timer", m_time_period, m_timercb );
}

Calendar::~Calendar( void )
{
    _DRAWSPACE_DELETE_( m_timercb ); 
}

dstime Calendar::GetOffsetTime( void )
{
    return m_offset_time;
}

Calendar::TimeMode Calendar::GetCurrentTimeFactor( void )
{
    return m_time_mode;
}

void Calendar::SetTimeFactor( Calendar::TimeMode p_time_mode )
{
    m_time_mode = p_time_mode;

    switch( p_time_mode )
    {
        case NORMAL_TIME:

            m_time_period = 1000;
            m_time_factor = 1;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;
            break;

        case MUL2_TIME:

            m_time_period = 500;
            m_time_factor = 2;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;
            break;

        case MUL4_TIME:
           
            m_time_period = 250;
            m_time_factor = 4;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;
            break;

        case MUL10_TIME:

            m_time_period = 100;
            m_time_factor = 10;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;
            break;

        case MUL100_TIME:

            m_time_period = 100;
            m_time_factor = 100.0;
            m_current_time_increment = 10;

            m_sub_sec_count_lim = 0;
            break;

        case MUL500_TIME:

            m_time_period = 100;
            m_time_factor = 500.0;
            m_current_time_increment = 50;

            m_sub_sec_count_lim = 0;
            break;


        case SEC_1HOUR_TIME:

            m_time_period = 100;
            m_time_factor = 3600.0;
            m_current_time_increment = 360;

            m_sub_sec_count_lim = 0;
            break;

        case SEC_1DAY_TIME:

            m_time_period = 100;
            m_time_factor = 86400.0;
            m_current_time_increment = 8640;

            m_sub_sec_count_lim = 0;
            break;

        case SEC_30DAYS_TIME:

            m_time_period = 100;
            m_time_factor = 2592000.0;
            m_current_time_increment = 259200;

            m_sub_sec_count_lim = 0;
            break;

        case SEC_1YEAR_TIME:

            m_time_period = 100;
            m_time_factor = 86400.0 * 365.0;
            m_current_time_increment = 86400 * 365;

            m_sub_sec_count_lim = 0;
            break;

        case DIV2_TIME:

            m_time_period = 1000;
            m_time_factor = 0.5;
            m_sub_sec_count_lim = 2;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;             
            break;

        case DIV4_TIME:

            m_time_period = 1000;
            m_time_factor = 0.25;
            m_sub_sec_count_lim = 4;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;
            break;

        case DIV10_TIME:

            m_time_period = 1000;
            m_time_factor = 0.1;
            m_sub_sec_count_lim = 10;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;
            break;
    }

    m_time_mode = p_time_mode;

    if( m_active && !m_freeze )
    {    
        m_time_manager->SetTimerState( "calendar_timer", false );
        m_time_manager->SetTimerPeriod( "calendar_timer", m_time_period );
        m_time_manager->SetTimerState( "calendar_timer", true );

    }
}

long Calendar::GetSubSecCount( void )
{
    return m_sub_sec_count;
}

void Calendar::GetFormatedDate( dsstring& p_date )
{
    struct tm* mytime;

    mytime = _gmtime64( &m_current_time );
    if( mytime )
    {
        p_date = asctime( mytime );
    }
    else
    {
        p_date = "???";
    }   
}

void Calendar::RegisterOrbit( Orbit* p_orbit )
{
    m_orbits.push_back( p_orbit );
}

void Calendar::set_orbit_angle( Orbit* p_orbit, dstime p_currtime )
{
    dsreal orbit_duration = p_orbit->m_orbit_duration;
    
    long year_sec = 3600 * 24 * 365;

    dsreal delta_time = ( (double) ( p_currtime - m_offset_time ) ) / year_sec;
    dsreal num_orbits = delta_time / orbit_duration;

    dsreal angle_orbits = num_orbits * 360;

    double angle_orbit_i, angle_orbit_f;
    long angle_orbit_i_2;

    angle_orbit_f = modf( angle_orbits, &angle_orbit_i );

    angle_orbit_i_2 = (long)angle_orbit_i;

    double final_angle = (angle_orbit_i_2 % 360) + angle_orbit_f;       
    
    p_orbit->m_orbit_angle = final_angle;
}

bool Calendar::Startup( dstime p_start_time )
{ 
    if( p_start_time < m_offset_time )
    {
        return false;
    }

    // positionner les orbites a leur angle de depart...

    for( size_t i = 0; i < m_orbits.size(); i++ )
    {
        Orbit* curr_orbit = m_orbits[i];

        set_orbit_angle( curr_orbit, p_start_time );
    }

    // demarre le timer...

    m_current_time = p_start_time;

    if( m_time_period != -1 )
    {
        m_time_manager->SetTimerPeriod( "calendar_timer", m_time_period );
        m_time_manager->SetTimerState( "calendar_timer", true );
    }

    m_active = true;

    return true;
}

void Calendar::Shutdown( void )
{
    m_time_manager->SetTimerState( "calendar_timer", false );

    m_active = false;
}

void Calendar::on_timer( dsstring p_timername )
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

void Calendar::Run( void )
{
    m_time_manager->Update();

    if( m_freeze )
    {
        return;
    }
    
    if( m_time_manager->IsReady() )
    {
        long fps = m_time_manager->GetFPS();
        long year_sec = 3600 * 24 * 365;

        for( size_t i = 0; i < m_orbits.size(); i++ )
        {
            Orbit* curr_orbit = m_orbits[i];
        
            set_orbit_angle( curr_orbit, m_current_time );
        }

        m_world->StepSimulation( m_time_manager->GetFPS() / m_time_factor ); 
    }
}

bool Calendar::IsTimerReady( void )
{
    return m_time_manager->IsReady();
}

dstime Calendar::GetCurrentInstant( void )
{
    return m_current_time;
}

void Calendar::Suspend( bool p_suspend )
{
    m_freeze = p_suspend;

    m_time_manager->SuspendTimer( "calendar_timer", p_suspend );
}