/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
#include "plugin.h"
#include "maths.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Commons;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Maths;

double RevolutionTransformAspectImpl::compute_revolution_angle(double p_revolution_duration, double p_curr_angle)
{
    double curr_angle   { p_curr_angle };
    int fps             { m_time_aspect->GetComponent<int>("output_fps")->getPurpose() };

    if (fps > 0)
    {
        TimeAspect::TimeScale time_scale{ m_time_aspect->GetComponent<TimeAspect::TimeScale>("time_scale")->getPurpose() };

        static const std::map< TimeAspect::TimeScale, dsreal> timefactor_array = {
            { TimeAspect::TimeScale::NORMAL_TIME, 1.0 },
            { TimeAspect::TimeScale::MUL2_TIME, 2.0 },
            { TimeAspect::TimeScale::MUL4_TIME, 4.0 },
            { TimeAspect::TimeScale::MUL10_TIME, 10.0 },
            { TimeAspect::TimeScale::MUL100_TIME, 100.0 },
            { TimeAspect::TimeScale::MUL500_TIME, 500.0 },
            { TimeAspect::TimeScale::SEC_1HOUR_TIME, 3600.0 },
            { TimeAspect::TimeScale::SEC_1DAY_TIME, 86400.0 },
            { TimeAspect::TimeScale::SEC_30DAYS_TIME, 2592000.0 },
            { TimeAspect::TimeScale::SEC_1YEAR_TIME, 31536000.0 },
            { TimeAspect::TimeScale::DIV2_TIME, 0.5 },
            { TimeAspect::TimeScale::DIV4_TIME, 0.25 },
            { TimeAspect::TimeScale::DIV10_TIME, 0.1 },
            { TimeAspect::TimeScale::FREEZE, 0.0 },
        };

        double den{ 24.0 * 3600.0 * p_revolution_duration * (double)fps };
        double step{ 360.0 / den };
        curr_angle += step * timefactor_array.at(time_scale);
    }
    return curr_angle;
}

void RevolutionTransformAspectImpl::GetLocaleTransform(Aspect::TransformAspect* p_transformaspect, Maths::Matrix& p_out_base_transform)
{
    dsreal angle{ p_transformaspect->GetComponent<dsreal>("revol_angle")->getPurpose()};
    dsreal revol_duration{ p_transformaspect->GetComponent<dsreal>("revol_duration")->getPurpose() }; //unite : 1.0 jour terrestre (24h)

    Matrix revol_rotation;
    revol_rotation.rotation(Vector(0.0, 1.0, 0.0, 1.0), Maths::degToRad(angle));

    p_out_base_transform = revol_rotation;
    dsreal revol_angle{ compute_revolution_angle(revol_duration, angle) };
    p_transformaspect->GetComponent<dsreal>("revol_angle")->getPurpose() = revol_angle;
}

