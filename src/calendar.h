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

#ifndef _CALENDAR_H_
#define _CALENDAR_H_



#include "world.h"
#include "body.h"
#include "orbiter.h"
#include "timemanager.h"

/*

31536000

nb annees calendaires (365j) pour une revolution
 -> nb secondes pour une revolution



 calendar :

   -> m_offset_time : instant "0"

   -> time_factor
   
   -> orbite : d�duire l'angle actuel en fonction de la date

   -> progression des orbites
   -> progression des mouvements
 

*/

#define BASE_TIMESTEP 8

namespace DrawSpace
{
namespace Dynamics
{
class Calendar
{
public:

    typedef enum
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
        DIV10_TIME

    } TimeMode;

protected:
    
    typedef DrawSpace::Core::CallBack<Calendar, void, DrawSpace::Utils::Timer*> CalendarTimer;

    dstime                                      m_offset_time;
    dstime                                      m_current_time;
    long                                        m_current_time_increment;

    TimeMode                                    m_time_mode;
    long                                        m_time_period;
    dsreal                                      m_time_factor;

    int                                         m_world_nbsteps;

    bool                                        m_freeze;

    long                                        m_sub_sec_count;
    long                                        m_sub_sec_count_lim;

    DrawSpace::Utils::TimeManager*              m_time_manager;
    DrawSpace::Utils::Timer                     m_timer;

    std::vector<Orbit*>                         m_orbits;
    std::vector<Orbiter*>                       m_orbiters;

    CalendarTimer*                              m_timercb;
    bool                                        m_active;

    std::vector<DrawSpace::Dynamics::World*>    m_worlds;

    void on_timer( DrawSpace::Utils::Timer* p_timer );

    double compute_orbit_angle( double p_orbit_duration, dstime p_currtime );
    double compute_revolution_angle( double p_revolution_duration, dstime p_currtime );

public:
    
    Calendar( dstime p_offset_time, DrawSpace::Utils::TimeManager* p_tm );
    virtual ~Calendar( void );

    dstime      GetOffsetTime( void );
    TimeMode    GetCurrentTimeFactor( void );
    void        SetTimeFactor( TimeMode p_time_mode );
    void        GetFormatedDate( dsstring& p_date );

    void        RegisterOrbit( Orbit* p_orbit );
    void        RegisterOrbiter( Orbiter* p_orbiter );
    
    bool        Startup( dstime p_start_time );
    void        Shutdown( void );

    void        Run( void );

    bool        IsTimerReady( void );

    dstime      GetCurrentInstant( void );

    long        GetSubSecCount( void );

    void        Suspend( bool p_suspend );

    void        RegisterWorld( DrawSpace::Dynamics::World* p_world );

    void        AngleSpeedInc( dsreal *p_angle, dsreal p_angleSpeed );
    void        AngleSpeedDec( dsreal *p_angle, dsreal p_angleSpeed );
    void        TranslationSpeedInc( dsreal *p_translation, dsreal p_speed );
    void        TranslationSpeedDec( dsreal *p_translation, dsreal p_speed );
        
};
}
}

#endif
