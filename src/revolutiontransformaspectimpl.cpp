/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

    ComponentList<dstime> times_count;
    m_time_aspect->GetComponentsByType<dstime>( times_count );
    dstime current_time = times_count[0]->getPurpose();
    dsreal revol_angle = compute_revolution_angle( revol_duration, current_time );

    revol_params[0]->getPurpose() = revol_angle;
}

double RevolutionTransformAspectImpl::compute_revolution_angle( double p_revolution_duration, dstime p_currtime )
{
    if( 0.0 == p_revolution_duration )
    {
        return 0.0;
    }

    dsreal revolution_duration = p_revolution_duration;

    long day_sec = 3600 * 24;

    dsreal delta_time = ( (double) ( p_currtime ) ) / day_sec;
    dsreal num_revs = delta_time / revolution_duration;

    dsreal angle_revs = num_revs * 360;

    double angle_rev_i, angle_rev_f;
    long angle_rev_i_2;

    angle_rev_f = modf( angle_revs, &angle_rev_i );

    angle_rev_i_2 = (long)angle_rev_i;

    double final_angle = (angle_rev_i_2 % 360) + angle_rev_f;       

    return final_angle;
}
