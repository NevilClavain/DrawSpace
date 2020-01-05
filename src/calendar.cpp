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

/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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


Calendar::Calendar( dstime p_offset_time, DrawSpace::Utils::TimeManager* p_tm ) : 
m_offset_time( p_offset_time ), 
m_time_mode( NORMAL_TIME ),
m_time_period( 1000 ),
m_time_factor( 1 ),
m_current_time_increment( 1 ),
m_time_manager( p_tm ),
m_active( false ),
//m_world( p_world ),
m_sub_sec_count( 0 ),
m_sub_sec_count_lim( 0 ),
m_freeze( false ),
m_world_nbsteps( 5 )
{
    m_current_time = m_offset_time;
    m_timercb = _DRAWSPACE_NEW_( CalendarTimer, CalendarTimer( this, &Calendar::on_timer ) );

    m_timer.SetHandler( m_timercb );
    m_timer.SetPeriod( m_time_period );
    p_tm->RegisterTimer( &m_timer );
}

Calendar::~Calendar( void )
{
    m_time_manager->UnregisterTimer( &m_timer );
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

            m_world_nbsteps = BASE_TIMESTEP;
            break;

        case MUL2_TIME:

            m_time_period = 500;
            m_time_factor = 2;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = BASE_TIMESTEP * 2;
            break;

        case MUL4_TIME:
           
            m_time_period = 250;
            m_time_factor = 4;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = BASE_TIMESTEP * 4;
            break;

        case MUL10_TIME:

            m_time_period = 100;
            m_time_factor = 10;
            m_current_time_increment = 1;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = BASE_TIMESTEP * 10;
            break;

        case MUL100_TIME:

            m_time_period = 100;
            m_time_factor = 100.0;
            m_current_time_increment = 10;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = BASE_TIMESTEP * 10;
            break;

        case MUL500_TIME:

            m_time_period = 100;
            m_time_factor = 500.0;
            m_current_time_increment = 50;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = BASE_TIMESTEP * 10;
            break;


        case SEC_1HOUR_TIME:

            m_time_period = 100;
            m_time_factor = 3600.0;
            m_current_time_increment = 360;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = BASE_TIMESTEP * 10;
            break;

        case SEC_1DAY_TIME:

            m_time_period = 100;
            m_time_factor = 86400.0;
            m_current_time_increment = 8640;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = BASE_TIMESTEP * 10;
            break;

        case SEC_30DAYS_TIME:

            m_time_period = 100;
            m_time_factor = 2592000.0;
            m_current_time_increment = 259200;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = 200;

            m_world_nbsteps = BASE_TIMESTEP * 10;
            break;

        case SEC_1YEAR_TIME:

            m_time_period = 100;
            m_time_factor = 86400.0 * 365.0;
            m_current_time_increment = 86400 * 365;

            m_sub_sec_count_lim = 0;

            m_world_nbsteps = BASE_TIMESTEP * 10;
            break;

        case DIV2_TIME:

            m_time_period = 1000;
            m_time_factor = 0.5;
            m_sub_sec_count_lim = 2;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = BASE_TIMESTEP;
            break;

        case DIV4_TIME:

            m_time_period = 1000;
            m_time_factor = 0.25;
            m_sub_sec_count_lim = 4;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = BASE_TIMESTEP;
            break;

        case DIV10_TIME:

            m_time_period = 1000;
            m_time_factor = 0.1;
            m_sub_sec_count_lim = 10;
            m_sub_sec_count = 0;

            m_current_time_increment = 0;

            m_world_nbsteps = BASE_TIMESTEP;
            break;
    }

    m_time_mode = p_time_mode;

    if( m_active && !m_freeze )
    {
        m_timer.SetState( false );
        m_timer.SetPeriod( m_time_period );
        m_timer.SetState( true );
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

void Calendar::RegisterOrbiter( Orbiter* p_orbiter )
{
    m_orbiters.push_back( p_orbiter );
}


double Calendar::compute_orbit_angle( double p_orbit_duration, dstime p_currtime )
{
    dsreal orbit_duration = p_orbit_duration;
    
    long year_sec = 3600 * 24 * 365;

    dsreal delta_time = ( (double) ( p_currtime - m_offset_time ) ) / year_sec;
    dsreal num_orbits = delta_time / orbit_duration;

    dsreal angle_orbits = num_orbits * 360;

    double angle_orbit_i, angle_orbit_f;
    long angle_orbit_i_2;

    angle_orbit_f = modf( angle_orbits, &angle_orbit_i );

    angle_orbit_i_2 = (long)angle_orbit_i;

    double final_angle = (angle_orbit_i_2 % 360) + angle_orbit_f;   
    
    return final_angle;
}

double Calendar::compute_revolution_angle( double p_revolution_duration, dstime p_currtime )
{
    if( 0.0 == p_revolution_duration )
    {
        return 0.0;
    }

    dsreal revolution_duration = p_revolution_duration;

    long day_sec = 3600 * 24;

    dsreal delta_time = ( (double) ( p_currtime - m_offset_time ) ) / day_sec;
    dsreal num_revs = delta_time / revolution_duration;

    dsreal angle_revs = num_revs * 360;

    double angle_rev_i, angle_rev_f;
    long angle_rev_i_2;

    angle_rev_f = modf( angle_revs, &angle_rev_i );

    angle_rev_i_2 = (long)angle_rev_i;

    double final_angle = (angle_rev_i_2 % 360) + angle_rev_f;       

    return final_angle;
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

        curr_orbit->m_orbit_angle = compute_orbit_angle( curr_orbit->m_orbit_duration, p_start_time );
        curr_orbit->m_revolution_angle = compute_revolution_angle( curr_orbit->m_revolution_duration, p_start_time );
    }

    for( size_t i = 0; i < m_orbiters.size(); i++ )
    {
        Orbiter* curr_orbiter = m_orbiters[i];

        curr_orbiter->m_orbit_angle = compute_orbit_angle( curr_orbiter->m_orbit_duration, p_start_time );
        curr_orbiter->m_revolution_angle = compute_revolution_angle( curr_orbiter->m_revolution_duration, p_start_time );
    }

    // demarre le timer...
    m_current_time = p_start_time;
    if( m_time_period != -1 )
    {

        m_timer.SetPeriod( m_time_period );
        m_timer.SetState( true );
    }
    m_active = true;

    return true;
}

void Calendar::Shutdown( void )
{
    m_timer.SetState( false );
    m_active = false;
}

void Calendar::on_timer( DrawSpace::Utils::Timer* p_timer )
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
        
            curr_orbit->m_orbit_angle = compute_orbit_angle( curr_orbit->m_orbit_duration, m_current_time );
            curr_orbit->m_revolution_angle = compute_revolution_angle( curr_orbit->m_revolution_duration, m_current_time );
        }

        for( size_t i = 0; i < m_orbiters.size(); i++ )
        {
            Orbiter* curr_orbiter = m_orbiters[i];

            curr_orbiter->m_orbit_angle = compute_orbit_angle( curr_orbiter->m_orbit_duration, m_current_time );
            curr_orbiter->m_revolution_angle = compute_revolution_angle( curr_orbiter->m_revolution_duration, m_current_time );
        }


        for( size_t i = 0; i < m_worlds.size(); i++ )
        {
            m_worlds[i]->StepSimulation( m_time_manager->GetFPS() / m_time_factor, m_world_nbsteps ); 
        }
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

    m_timer.Suspend( p_suspend );
}

void Calendar::RegisterWorld( DrawSpace::Dynamics::World* p_world )
{
    m_worlds.push_back( p_world );
}

void Calendar::UnregisterWorld( DrawSpace::Dynamics::World* p_world )
{
	for( auto it = m_worlds.begin(); it!= m_worlds.end(); ++it )
	{
		if( (*it) == p_world )
		{
			m_worlds.erase( it );
			return;
		}
	}
}

void Calendar::AngleSpeedInc( dsreal *p_angle, dsreal p_angleSpeed )
{
    m_time_manager->AngleSpeedInc( p_angle, p_angleSpeed * m_time_factor );
}

void Calendar::AngleSpeedDec( dsreal *p_angle, dsreal p_angleSpeed )
{
    m_time_manager->AngleSpeedDec( p_angle, p_angleSpeed * m_time_factor );
}

void Calendar::TranslationSpeedInc( dsreal *p_translation, dsreal p_speed )
{
    m_time_manager->TranslationSpeedInc( p_translation, p_speed * m_time_factor );
}

void Calendar::TranslationSpeedDec( dsreal *p_translation, dsreal p_speed )
{
    m_time_manager->TranslationSpeedDec( p_translation, p_speed * m_time_factor );
}
