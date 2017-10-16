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

#include "freetransformaspectimpl.h"
#include "component.h"
#include "transformaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

FreeTransformAspectImpl::FreeTransformAspectImpl( TimeManager& p_timemanager ) :
m_timemanager( p_timemanager )
{
}



void FreeTransformAspectImpl::GetLocaleTransform( DrawSpace::Aspect::TransformAspect* p_transformaspect, Utils::Matrix& p_out_base_transform )
{
    
    // recup des composants donnees d'entr�es

    ComponentList<Vector> vectors;
    p_transformaspect->GetComponentsByType<Vector>( vectors );

    Vector local_speed = vectors[0]->getPurpose();
    local_speed[2] = -local_speed[2];

    ComponentList<Matrix> mats;
    p_transformaspect->GetComponentsByType<Matrix>( mats );

    Matrix pos = mats[0]->getPurpose();

    //vitesses demand�es...

    ComponentList<dsreal> reals;
    p_transformaspect->GetComponentsByType<dsreal>( reals );
    dsreal rspeed_x = reals[0]->getPurpose();
    dsreal rspeed_y = reals[1]->getPurpose();
    dsreal rspeed_z = reals[2]->getPurpose();

    // axe demand�
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

    fps = m_timemanager.ConvertUnitPerSecFramePerSec( rspeed_x );
	q.RotationAxis( rot_axis_x, fps );
    qres = current_res * q;
    current_res = qres;

    current_res.RotationMatFrom( orientation );
    rot_axis_x[0] = orientation( 0, 0 );
    rot_axis_x[1] = orientation( 0, 1 );
    rot_axis_x[2] = orientation( 0, 2 );

    /////////////////Axe Y /////////////////////////////

    fps = m_timemanager.ConvertUnitPerSecFramePerSec( rspeed_y );
	q.RotationAxis( rot_axis_y, fps );
    qres = current_res * q;
    current_res = qres;

    current_res.RotationMatFrom( orientation );
    rot_axis_y[0] = orientation( 1, 0 );
    rot_axis_y[1] = orientation( 1, 1 );
    rot_axis_y[2] = orientation( 1, 2 );

    /////////////////Axe Z /////////////////////////////

    fps = m_timemanager.ConvertUnitPerSecFramePerSec( rspeed_z );
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
    m_timemanager.TranslationSpeedInc( &pos( 3, 0 ), gs[0] );
    m_timemanager.TranslationSpeedInc( &pos( 3, 1 ), gs[1] );
    m_timemanager.TranslationSpeedInc( &pos( 3, 2 ), gs[2] );

    p_out_base_transform = orientation * pos;

    mats[0]->getPurpose() = pos;
    quats[0]->getPurpose() = current_res;

    vectors[1]->getPurpose() = rot_axis_x;
    vectors[2]->getPurpose() = rot_axis_y;
    vectors[3]->getPurpose() = rot_axis_z;
}
