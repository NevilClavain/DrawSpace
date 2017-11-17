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

#include "orbittransformaspectimpl.h"
#include "component.h"
#include "transformaspect.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

void OrbitTransformAspectImpl::GetLocaleTransform( TransformAspect* p_transformaspect, Matrix& p_out_base_transform )
{
    ComponentList<dsreal> orbit_params;
    p_transformaspect->GetComponentsByType<dsreal>( orbit_params );

    dsreal orbit_ray = orbit_params[0]->getPurpose();
    dsreal excentricity = orbit_params[1]->getPurpose();
    dsreal angle = orbit_params[2]->getPurpose();
    dsreal orbit_duration = orbit_params[3]->getPurpose();  // duree de parcours complet d'une orbite : 1.0 = 1 annee terrestre (365 jours)
    dsreal orbit_offset_rot = orbit_params[4]->getPurpose();

    dsreal a = 1.0;
    dsreal b = excentricity;

    dsreal rad_ang = Maths::DegToRad( angle );
    dsreal x = a * cos( rad_ang );
    dsreal z = b * sin( rad_ang );

    x = ( x * orbit_ray );
    z = ( z * orbit_ray );

    Matrix orbit;
    orbit.Translation( x, 0.0, z );

    Matrix sync_rot;
    sync_rot.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( 360.0 - angle ) );


    p_out_base_transform = sync_rot * orbit;


    ComponentList<dstime> times_count;
    m_time_aspect->GetComponentsByType<dstime>( times_count );
    dstime current_time = times_count[0]->getPurpose();
    dsreal orbit_angle = compute_orbit_angle( orbit_duration, current_time );

    orbit_params[2]->getPurpose() = orbit_angle;
}

dsreal OrbitTransformAspectImpl::compute_orbit_angle( dsreal p_orbit_duration, dstime p_currtime )
{
    dsreal orbit_duration = p_orbit_duration;
    
    long year_sec = 3600 * 24 * 365;

    dsreal delta_time = ( (dsreal) ( p_currtime ) ) / year_sec;
    dsreal num_orbits = delta_time / orbit_duration;

    dsreal angle_orbits = num_orbits * 360;

    dsreal angle_orbit_i, angle_orbit_f;
    long angle_orbit_i_2;

    angle_orbit_f = modf( angle_orbits, &angle_orbit_i );

    angle_orbit_i_2 = (long)angle_orbit_i;

    dsreal final_angle = (angle_orbit_i_2 % 360) + angle_orbit_f;   
    
    return final_angle;
}