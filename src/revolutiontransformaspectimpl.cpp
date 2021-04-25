/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "revolutiontransformaspectimpl.h"
#include "component.h"
#include "transformaspect.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

void RevolutionTransformAspectImpl::GetLocaleTransform( TransformAspect* p_transformaspect, Matrix& p_out_base_transform )
{
    ComponentList<dsreal> revol_params;
    p_transformaspect->GetComponentsByType<dsreal>( revol_params );

    dsreal angle = revol_params[0]->getPurpose();
    dsreal revol_duration = revol_params[1]->getPurpose(); //unite : 1.0 jour terrestre (24h)

    Matrix revol_rotation;

    revol_rotation.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( angle ) );

    p_out_base_transform = revol_rotation;

    dsreal revol_angle = compute_revolution_angle(revol_duration, angle);

    revol_params[0]->getPurpose() = revol_angle;
}

double RevolutionTransformAspectImpl::compute_revolution_angle(double p_revolution_duration, double p_curr_angle)
{
    double curr_angle{ p_curr_angle };
    
    int fps{ m_time_aspect->GetComponent<int>("output_fps")->getPurpose() };
    if (fps > 0)
    {
        TimeAspect::TimeScale time_scale{ m_time_aspect->GetComponent<TimeAspect::TimeScale>("time_scale")->getPurpose() };

        static const std::map< TimeAspect::TimeScale, dsreal> timefactor_array = {

            { TimeAspect::NORMAL_TIME, 1.0 },
            { TimeAspect::MUL2_TIME, 2.0 },
            { TimeAspect::MUL4_TIME, 4.0 },
            { TimeAspect::MUL10_TIME, 10.0 },
            { TimeAspect::MUL100_TIME, 100.0 },
            { TimeAspect::MUL500_TIME, 500.0 },
            { TimeAspect::SEC_1HOUR_TIME, 3600.0 },
            { TimeAspect::SEC_1DAY_TIME, 86400.0 },
            { TimeAspect::SEC_30DAYS_TIME, 2592000.0 },
            { TimeAspect::SEC_1YEAR_TIME, 31536000.0 },
            { TimeAspect::DIV2_TIME, 0.5 },
            { TimeAspect::DIV4_TIME, 0.25 },
            { TimeAspect::DIV10_TIME, 0.1 },
            { TimeAspect::FREEZE, 0.0 },

        };

        double den{ 24.0 * 3600.0 * p_revolution_duration * (double)fps };
        double step{ 360.0 / den };
        curr_angle += step * timefactor_array.at(time_scale);
    }    
    return curr_angle;
}
