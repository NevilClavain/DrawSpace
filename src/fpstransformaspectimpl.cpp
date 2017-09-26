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

#include "fpstransformaspectimpl.h"
#include "component.h"
#include "worldaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

FPSTransformAspectImpl::FPSTransformAspectImpl( TimeManager& p_timemanager ) :
m_timemanager( p_timemanager )
{
    m_position.Identity();
}

void FPSTransformAspectImpl::GetLocaleTransform( WorldAspect* p_worldaspect, Utils::Matrix& p_out_base_transform )
{
    // recup des composants donnees d'entrées
    ComponentList<dsreal> angles;
    p_worldaspect->GetComponentsByType<dsreal>( angles );

    dsreal angle_yaw = angles[0]->getPurpose();
    dsreal angle_pitch = angles[1]->getPurpose();

    ComponentList<Vector> vectors;
    p_worldaspect->GetComponentsByType<Vector>( vectors );

    Vector local_speed = vectors[0]->getPurpose();
    Vector init_pos = vectors[1]->getPurpose();

    ComponentList<bool> flags;
    p_worldaspect->GetComponentsByType<bool>( flags );

    bool y_mvt = flags[0]->getPurpose();

	// les quaternions
    Utils::Quaternion		    qyaw;
	Utils::Quaternion		    qpitch;
    Utils::Quaternion		    current_res;

    // les sorties
    //Utils::Matrix			    position;
    Utils::Matrix			    orientation;

	Vector gs;

	Vector yaxis( 0.0, 1.0, 0.0, 1.0 );
	Vector xaxis( 1.0, 0.0, 0.0, 1.0 );

    //position.Identity();

	qyaw.RotationAxis( yaxis, angle_yaw );
	qpitch.RotationAxis( xaxis, angle_pitch );

	current_res = qpitch * qyaw;
	current_res.RotationMatFrom( orientation );

    orientation.Transform( &local_speed, &gs );

	m_timemanager.TranslationSpeedInc( &m_position( 3, 0 ), gs[0] );

    
	if( y_mvt )
	{
		// prendre aussi en compte la composante en Y (la camera peut aussi evoluer "en hauteur")
		m_timemanager.TranslationSpeedInc( &m_position( 3, 1 ), gs[1] );
	}
    

	m_timemanager.TranslationSpeedInc( &m_position( 3, 2 ), gs[2] );

    //m_position( 3, 0 ) += init_pos[0];
    //m_position( 3, 1 ) += init_pos[1];
    //m_position( 3, 2 ) += init_pos[2];

    p_out_base_transform = orientation * m_position;
}