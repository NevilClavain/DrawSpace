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
m_world( p_world )
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

Calendar::TimeFactor Calendar::GetCurrentTimeFactor( void )
{
    return m_time_mode;
}

void Calendar::SetTimeFactor( Calendar::TimeFactor p_time_factor )
{
    TimeFactor previous_time_factor = m_time_mode;

    m_time_mode = p_time_factor;

    switch( m_time_mode )
    {
        case NORMAL_TIME:

            m_time_period = 1000;
            m_time_factor = 1;
            m_current_time_increment = 1;
            break;

        case MUL2_TIME:

            m_time_period = 500;
            m_time_factor = 2;
            m_current_time_increment = 1;
            break;

        case MUL4_TIME:
           
            m_time_period = 250;
            m_time_factor = 4;
            m_current_time_increment = 1;
            break;

        case MUL10_TIME:

            m_time_period = 100;
            m_time_factor = 10;
            m_current_time_increment = 1;
            break;

        case SEC_1HOUR_TIME:

            m_time_period = 100;
            m_time_factor = 3600;
            m_current_time_increment = 360;
            break;

        case SEC_1DAY_TIME:

            m_time_period = 100;
            m_time_factor = 86400;
            m_current_time_increment = 8640;
            break;

        case SEC_30DAYS_TIME:

            m_time_period = 100;
            m_time_factor = 2592000;
            m_current_time_increment = 259200;
            break;

        case PAUSE_TIME:

            m_time_period = -1;
            break;
    }

    if( m_active )
    {    
        if( m_time_period != -1 )
        {
            m_time_manager->SetTimerState( "calendar_timer", false );
            m_time_manager->SetTimerPeriod( "calendar_timer", m_time_period );
            m_time_manager->SetTimerState( "calendar_timer", true );
        }
    }
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

bool Calendar::Startup( int p_sec, int p_min, int p_hour, int p_day, int p_month, int p_year )
{
    struct tm mytime;
    mytime.tm_hour = p_hour;
    mytime.tm_min = p_min;
    mytime.tm_sec = p_sec;
    mytime.tm_year = p_year;
    mytime.tm_mon = p_month;
    mytime.tm_mday = p_day;
    mytime.tm_wday = mytime.tm_yday = 0;

    return Startup( _mktime64( &mytime ) );
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

        dsreal orbit_duration = curr_orbit->m_orbit_duration;
        
        long year_sec = 3600 * 24 * 365;

        dsreal delta_time = ( (double) ( p_start_time - m_offset_time ) ) / year_sec;
        dsreal num_orbits = delta_time / orbit_duration;

        dsreal angle_orbits = num_orbits * 360;

        double angle_orbit_i, angle_orbit_f;
        long angle_orbit_i_2;

        angle_orbit_f = modf( angle_orbits, &angle_orbit_i );

        angle_orbit_i_2 = (long)angle_orbit_i;

        double final_angle = (angle_orbit_i_2 % 360) + angle_orbit_f;       
        
        curr_orbit->m_orbit_angle = final_angle;
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
    if( PAUSE_TIME == m_time_mode )
    {
        return;
    }

    m_current_time += m_current_time_increment;
}

void Calendar::Run( void )
{
    m_time_manager->Update();

    if( PAUSE_TIME == m_time_mode )
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
        
            // TO BE CONTINUED... HERE
            /*
            // calcul vitesse angulaire
            dsreal p = year_sec * curr_orbit->m_orbit_duration;
            dsreal va =  360.0 / p;

            m_time_manager->AngleSpeedInc( &curr_orbit->m_orbit_angle, va * m_time_factor );
            */

        }
        m_world->StepSimulation( m_time_manager->GetFPS() / m_time_factor ); 
    }
}

bool Calendar::IsTimerReady( void )
{
    return m_time_manager->IsReady();
}