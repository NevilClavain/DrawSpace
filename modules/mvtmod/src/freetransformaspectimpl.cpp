/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "freetransformaspectimpl.h"
#include "component.h"
#include "transformaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

FreeTransformAspectImpl::FreeTransformAspectImpl( void )
{
}



void FreeTransformAspectImpl::GetLocaleTransform( DrawSpace::Aspect::TransformAspect* p_transformaspect, Utils::Matrix& p_out_base_transform )
{
    if( NULL == m_time_aspect )
    {
        _DSEXCEPTION( "Need a time reference!!!" )
    }    

    // recup des composants donnees d'entr�es

    ComponentList<Vector> vectors;
    p_transformaspect->GetComponentsByType<Vector>( vectors );

    Vector local_speed = vectors[0]->getPurpose();
    local_speed[2] = -local_speed[2];

    ComponentList<Matrix> mats;
    p_transformaspect->GetComponentsByType<Matrix>( mats );

    Matrix pos = mats[0]->getPurpose();

    ComponentList<TimeAspect::TimeScale> time_scales;
    m_time_aspect->GetComponentsByType<TimeAspect::TimeScale>( time_scales );


    //vitesses demandées...

    ComponentList<dsreal> reals;
    p_transformaspect->GetComponentsByType<dsreal>( reals );
    dsreal rspeed_x = reals[0]->getPurpose();
    dsreal rspeed_y = reals[1]->getPurpose();
    dsreal rspeed_z = reals[2]->getPurpose();

    if( TimeAspect::FREEZE == time_scales[0]->getPurpose() )
    {
        rspeed_x = rspeed_y = rspeed_z = 0.0;    
    }


    // axe demandé
    Vector rot_axis_x = vectors[1]->getPurpose();
    Vector rot_axis_y = vectors[2]->getPurpose();    
    Vector rot_axis_z = vectors[3]->getPurpose();

    // quaternion resultat courant
    ComponentList<Quaternion>   quats;
    p_transformaspect->GetComponentsByType<Quaternion>( quats );
    Quaternion	                current_res = quats[0]->getPurpose();
    
    Utils::Matrix			    orientation;
    Vector                      gs;

    ////////////////////////////////////////


	Quaternion q, qres;    
    dsreal fps;


    /// NB: l'ordre dans lequel sont traites les axes n'a pas d'importance...

    /////////////////Axe X /////////////////////////////

    //fps = tm->ConvertUnitPerSecFramePerSec( rspeed_x );
    fps = m_time_aspect->ConvertUnitPerSecFramePerSec( rspeed_x );
	q.RotationAxis( rot_axis_x, fps );
    qres = current_res * q;
    current_res = qres;

    current_res.RotationMatFrom( orientation );
    rot_axis_x[0] = orientation( 0, 0 );
    rot_axis_x[1] = orientation( 0, 1 );
    rot_axis_x[2] = orientation( 0, 2 );

    /////////////////Axe Y /////////////////////////////

    //fps = tm->ConvertUnitPerSecFramePerSec( rspeed_y );
    fps = m_time_aspect->ConvertUnitPerSecFramePerSec( rspeed_y );
	q.RotationAxis( rot_axis_y, fps );
    qres = current_res * q;
    current_res = qres;

    current_res.RotationMatFrom( orientation );
    rot_axis_y[0] = orientation( 1, 0 );
    rot_axis_y[1] = orientation( 1, 1 );
    rot_axis_y[2] = orientation( 1, 2 );

    /////////////////Axe Z /////////////////////////////

    //fps = tm->ConvertUnitPerSecFramePerSec( rspeed_z );
    fps = m_time_aspect->ConvertUnitPerSecFramePerSec( rspeed_z );
	q.RotationAxis( rot_axis_z, fps );
    qres = current_res * q;
    current_res = qres;

    current_res.RotationMatFrom( orientation );
    rot_axis_z[0] = orientation( 2, 0 );
    rot_axis_z[1] = orientation( 2, 1 );
    rot_axis_z[2] = orientation( 2, 2 );


    // resultat calcul est dans 'orientation'

    ////////////////////////////////////////

    orientation.Transform( &local_speed, &gs );

    TimeAspect::TimeScalar pos_30 = m_time_aspect->TimeScalarFactory( pos( 3, 0 ) );
    TimeAspect::TimeScalar pos_31 = m_time_aspect->TimeScalarFactory( pos( 3, 1 ) );
    TimeAspect::TimeScalar pos_32 = m_time_aspect->TimeScalarFactory( pos( 3, 2 ) );

    pos_30 += gs[0];
    pos_31 += gs[1];
    pos_32 += gs[2];

    pos( 3, 0 ) = pos_30.GetValue();
    pos( 3, 1 ) = pos_31.GetValue();
    pos( 3, 2 ) = pos_32.GetValue();

    p_out_base_transform = orientation * pos;

    mats[0]->getPurpose() = pos;
    quats[0]->getPurpose() = current_res;

    vectors[1]->getPurpose() = rot_axis_x;
    vectors[2]->getPurpose() = rot_axis_y;
    vectors[3]->getPurpose() = rot_axis_z;
}

