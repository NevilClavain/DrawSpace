/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "timeaspect.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;

TimeAspect::TimeAspect( void ) : 
m_time_factor( 1.0 )
{
}

TimeAspect::TimeAngle TimeAspect::TimeAngleFactory( dsreal p_initvalue )
{
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

    return TimeAngle( p_initvalue, tm, &m_time_factor );
}


TimeAspect::TimeScalar TimeAspect::TimeScalarFactory( dsreal p_initvalue )
{
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

    return TimeScalar( p_initvalue, tm, &m_time_factor );
}

dsreal TimeAspect::ConvertUnitPerSecFramePerSec( dsreal p_speed )
{
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

    return tm->ConvertUnitPerSecFramePerSec( p_speed ) * m_time_factor;
}