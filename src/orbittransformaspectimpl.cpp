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



    TimeAspect::TimeScalar orbit_angle_evo = m_time_aspect->TimeScalarFactory( angle );

    orbit_angle_evo += 20.0;

    orbit_params[2]->getPurpose() = orbit_angle_evo.GetValue();
}