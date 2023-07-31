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

#include "orbittransformaspectimpl.h"
#include "plugin.h"
#include "transformaspect.h"
#include "matrix.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Commons;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Maths;

void OrbitTransformAspectImpl::GetLocaleTransform(Aspect::TransformAspect* p_transformaspect, Maths::Matrix& p_out_base_transform)
{

    dsreal orbit_ray{ p_transformaspect->GetComponent<dsreal>("orbit_ray")->getPurpose() };
    dsreal excentricity{ p_transformaspect->GetComponent<dsreal>("orbit_excentricity")->getPurpose() };
    dsreal orbit_angle{ p_transformaspect->GetComponent<dsreal>("orbit_angle")->getPurpose() };
    dsreal orbit_duration{ p_transformaspect->GetComponent<dsreal>("orbit_duration")->getPurpose() };
    dsreal orbit_offset_rot{ p_transformaspect->GetComponent<dsreal>("orbit_offset_rot")->getPurpose() };
    dsreal orbit_pan_angle{ p_transformaspect->GetComponent<dsreal>("orbit_pan_angle")->getPurpose() };
    dsreal orbit_tilt_angle{ p_transformaspect->GetComponent<dsreal>("orbit_tilt_angle")->getPurpose() };
    dsreal revol_axe_inclination{ p_transformaspect->GetComponent<dsreal>("revol_axe_inclination")->getPurpose() };


    orbit_angle += orbit_offset_rot;

    dsreal a = 1.0;
    dsreal b = excentricity;

    dsreal rad_ang = Maths::degToRad(orbit_angle);
    dsreal x = a * cos(rad_ang);
    dsreal z = b * sin(rad_ang);

    x = (x * orbit_ray);
    z = (z * orbit_ray);

    Matrix orbit;
    orbit.translation(x, 0.0, z);

    Matrix revol_ax;
    revol_ax.rotation(Vector(1.0, 0.0, 0.0, 1.0), Maths::degToRad(revol_axe_inclination)); // inclinaison de l'objet en orbite ( les saisons, pour une planete !!)

    Matrix orbit_tilt;
    orbit_tilt.rotation(Vector(0.0, 0.0, 1.0, 1.0), Maths::degToRad(orbit_tilt_angle));

    Matrix orbit_pan;
    orbit_pan.rotation(Vector(0.0, 1.0, 0.0, 1.0), Maths::degToRad(orbit_pan_angle));
    orbit_pan.rotation(Vector(0.0, 1.0, 0.0, 1.0), Maths::degToRad(orbit_pan_angle));

    p_out_base_transform = revol_ax * orbit * orbit_tilt * orbit_pan;

    ComponentList<dstime> times_count;
    m_time_aspect->GetComponentsByType<dstime>(times_count);
    dstime current_time = times_count[0]->getPurpose();
    orbit_angle = compute_orbit_angle(orbit_duration, current_time);

    p_transformaspect->GetComponent<dsreal>("orbit_angle")->getPurpose() = orbit_angle;
}

dsreal OrbitTransformAspectImpl::compute_orbit_angle(dsreal p_orbit_duration, dstime p_currtime)
{
    dsreal orbit_duration = p_orbit_duration;

    long year_sec = 3600 * 24 * 365;

    dsreal delta_time = ((dsreal)(p_currtime)) / year_sec;
    dsreal num_orbits = delta_time / orbit_duration;

    dsreal angle_orbits = num_orbits * 360;

    dsreal angle_orbit_i, angle_orbit_f;
    long angle_orbit_i_2;

    angle_orbit_f = modf(angle_orbits, &angle_orbit_i);

    angle_orbit_i_2 = (long)angle_orbit_i;

    dsreal final_angle = (angle_orbit_i_2 % 360) + angle_orbit_f;

    return final_angle;
}
