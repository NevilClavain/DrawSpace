/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#ifndef _TIMEASPECT_H_
#define _TIMEASPECT_H_

#include "componentcontainer.h"
#include "timemanager.h"

namespace DrawSpace
{
namespace Aspect
{
class TimeAspect : public Core::ComponentContainer
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

    class TimeAngle
    {
    private:
        dsreal                  m_value;
        Utils::TimeManager*     m_tm;
        dsreal*                 m_timefactor;
        bool*                   m_freeze;

        // seul TimeAspect appelle ce ctor
        TimeAngle( dsreal p_initval, Utils::TimeManager* p_tm, dsreal* p_timefactor, bool* p_freeze ) :
        m_value( p_initval ),
        m_tm( p_tm ),
        m_timefactor( p_timefactor ),
        m_freeze( p_freeze )
        {        
        }

    public:
        
        TimeAngle( void ) :
        m_value( 0.0 ),
        m_tm( NULL ),
        m_timefactor( NULL ),
        m_freeze( NULL )
        {
        }
        
        void Increase( dsreal p_delta )
        {
            if( NULL == m_freeze )
            {
                _DSEXCEPTION( "TimeAngle object must be initialized from TimeAngleFactory")
            }

            if( true == *m_freeze )
            {
                return;
            }

            if( m_tm->IsReady() )
            {
                m_tm->AngleSpeedInc( &m_value, p_delta * (*m_timefactor) );
            }
        }
        void Decrease( dsreal p_delta )
        {
            if( NULL == m_freeze )
            {
                _DSEXCEPTION( "TimeAngle object must be initialized from TimeAngleFactory")
            }

            if( true == *m_freeze )
            {
                return;
            }

            if( m_tm->IsReady() )
            {
                m_tm->AngleSpeedDec( &m_value, p_delta * (*m_timefactor) );
            }
        }
        void operator +=( dsreal p_delta )
        {
            Increase( p_delta );
        }
        void operator -=( dsreal p_delta )
        {
            Decrease( p_delta );
        }

        dsreal GetValue( void ) const { return m_value; };

        friend class TimeAspect;
    };

    class TimeScalar
    {
    private:
        dsreal                  m_value;
        Utils::TimeManager*     m_tm;
        dsreal*                 m_timefactor;
        bool*                   m_freeze;

        // seul TimeAspect appelle ce ctor
        TimeScalar( dsreal p_initval, Utils::TimeManager* p_tm, dsreal* p_timefactor, bool* p_freeze ) :
        m_value( p_initval ),
        m_tm( p_tm ),
        m_timefactor( p_timefactor ),
        m_freeze( p_freeze )
        {        
        }

    public:

        
        TimeScalar( void ) :
        m_value( 0.0 ),
        m_tm( NULL ),
        m_timefactor( NULL ),
        m_freeze( NULL )
        {
        }
       
        void Increase( dsreal p_delta )
        {
            if( NULL == m_freeze )
            {
                _DSEXCEPTION( "TimeScalar object must be initialized from TimeScalarFactory")
            }

            if( true == *m_freeze )
            {
                return;
            }

            if( m_tm->IsReady() )
            {
                m_tm->TranslationSpeedInc( &m_value, p_delta * (*m_timefactor) );
            }
        }
        void Decrease( dsreal p_delta )
        {
            if( NULL == m_freeze )
            {
                _DSEXCEPTION( "TimeScalar object must be initialized from TimeScalarFactory")
            }

            if( true == *m_freeze )
            {
                return;
            }

            if( m_tm->IsReady() )
            {
                m_tm->TranslationSpeedDec( &m_value, p_delta * (*m_timefactor) );
            }
        }
        void operator +=( dsreal p_delta )
        {
            Increase( p_delta );
        }
        void operator -=( dsreal p_delta )
        {
            Decrease( p_delta );
        }

        dsreal GetValue( void ) const { return m_value; };

        friend class TimeAspect;
    };

	class TimeMark
	{
	private:
		Utils::TimeManager*		m_tm;
		dsreal*					m_timefactor;
		bool*					m_freeze;
		long                    m_previous_tick; // for freeze case
		long					m_timecounter;

		// seul TimeAspect appelle ce ctor
		TimeMark(Utils::TimeManager* p_tm, dsreal* p_timefactor, bool* p_freeze) :			
			m_tm(p_tm),
			m_timefactor(p_timefactor),
			m_freeze(p_freeze),
			m_previous_tick(0),
			m_timecounter(0)
		{
		}

	public:
		TimeMark(void) :
			m_tm(NULL),
			m_timefactor(NULL),
			m_freeze(NULL),
			m_previous_tick(0),
			m_timecounter(0)
		{
		}

		void Reset(void);
		long ComputeTimeMs(void);

		friend class TimeAspect;
	};

protected:

    typedef DrawSpace::Core::CallBack<TimeAspect, void, DrawSpace::Utils::Timer*> TimerCb;

    static const int        m_base_timestep = 8;

    Utils::TimeManager*     m_tm;    
    TimeScale               m_mode;
    dsreal                  m_time_factor;  // calcul...

    TimerCb*                m_timercb;
    DrawSpace::Utils::Timer m_timer;
    long                    m_time_period;
    bool                    m_active;

    dstime                  m_current_time;
    long                    m_current_time_increment;

    long                    m_sub_sec_count;
    long                    m_sub_sec_count_lim;

    int                     m_world_nbsteps;

    bool                    m_freeze;

    void get_tm( void );

    void on_timer( DrawSpace::Utils::Timer* p_timer );

    void set_time_factor( TimeAspect::TimeScale p_scale );

public:


    TimeAspect( void );
    ~TimeAspect( void );
    
    TimeAngle	TimeAngleFactory( dsreal p_initvalue );
    TimeScalar	TimeScalarFactory( dsreal p_initvalue );
	TimeMark	TimeMarkFactory( void );

    dsreal ConvertUnitPerSecFramePerSec( dsreal p_speed );
    
    void Update( void );

    void Activate( void );
    void Deactivate( void );

};
}
}

#endif
