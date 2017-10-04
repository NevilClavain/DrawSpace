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
#include "worldaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

FreeTransformAspectImpl::FreeTransformAspectImpl( TimeManager& p_timemanager ) :
m_timemanager( p_timemanager )
{
}

void FreeTransformAspectImpl::Init( const Utils::Vector& p_init_pos )
{
	// init du quaternion resultat
	m_current_res.Identity();

	// init des axes tournants	
	m_current_x_axis[0] = 1.0;
	m_current_x_axis[1] = 0.0;
	m_current_x_axis[2] = 0.0;
	m_current_x_axis[3] = 1.0;

	m_current_y_axis[0] = 0.0;
	m_current_y_axis[1] = 1.0;
	m_current_y_axis[2] = 0.0;
	m_current_y_axis[3] = 1.0;

	m_current_z_axis[0] = 0.0;
	m_current_z_axis[1] = 0.0;
	m_current_z_axis[2] = 1.0;
	m_current_z_axis[3] = 1.0;
		
	// init des matrices resultats
    m_position.Translation( p_init_pos );
	m_orientation.Identity();
}

void FreeTransformAspectImpl::SetSpeed( dsreal p_speed )
{
    m_local_speed[2] = -p_speed;
}

void FreeTransformAspectImpl::RotateYaw( dsreal p_rspeed )
{
	Quaternion q, qres;

	dsreal fps = m_timemanager.ConvertUnitPerSecFramePerSec( p_rspeed );
	
	q.RotationAxis( m_current_y_axis, fps );
	qres = m_current_res * q;
	m_current_res = qres;

	// maj de la matrice resultat
	m_current_res.RotationMatFrom( m_orientation );

	// maj de l'axe
    
	m_current_y_axis[0] = m_orientation( 1, 0 );
	m_current_y_axis[1] = m_orientation( 1, 1 );
	m_current_y_axis[2] = m_orientation( 1, 2 );
    
}

void FreeTransformAspectImpl::RotatePitch( dsreal p_rspeed )
{
	Quaternion q, qres;

	dsreal fps = m_timemanager.ConvertUnitPerSecFramePerSec( p_rspeed );

	q.RotationAxis( m_current_x_axis, fps );
	qres = m_current_res * q;
	m_current_res = qres;

	// maj de la matrice resultat
	m_current_res.RotationMatFrom( m_orientation );

	// maj de l'axe
    
	m_current_x_axis[0] = m_orientation( 0, 0 );
	m_current_x_axis[1] = m_orientation( 0, 1 );
	m_current_x_axis[2] = m_orientation( 0, 2 );
    
}

void FreeTransformAspectImpl::GetLocaleTransform( WorldAspect* p_worldaspect, Utils::Matrix& p_out_base_transform )
{
    /*
    // recup des composants donnees d'entrées

    ComponentList<Vector> vectors;
    p_worldaspect->GetComponentsByType<Vector>( vectors );

    Vector local_speed = vectors[0]->getPurpose();
    local_speed[2] = -local_speed[2];

    ComponentList<Matrix> mats;
    p_worldaspect->GetComponentsByType<Matrix>( mats );

    Matrix pos = mats[0]->getPurpose();

    //vitesse demandée...

    ComponentList<dsreal> reals;
    p_worldaspect->GetComponentsByType<dsreal>( reals );
    dsreal rspeed0 = reals[0]->getPurpose();
    dsreal rspeed1 = reals[1]->getPurpose();
    dsreal rspeed2 = reals[2]->getPurpose();

    // axe demandé
    Vector rot_axis0 = vectors[1]->getPurpose();
    Vector rot_axis1 = vectors[2]->getPurpose();
    Vector rot_axis2 = vectors[3]->getPurpose();

    // quaternion resultat courant
    ComponentList<Quaternion>   quats;
    p_worldaspect->GetComponentsByType<Quaternion>( quats );
    Quaternion	                current_res = quats[0]->getPurpose();


    
    Utils::Matrix			    orientation;
    Vector                      gs;

    ////////////////////////////////////////


	Quaternion q, qres;    
    dsreal fps;

    fps = m_timemanager.ConvertUnitPerSecFramePerSec( rspeed0 );
	q.RotationAxis( rot_axis0, fps );
    qres = current_res * q;
    current_res = qres;

    fps = m_timemanager.ConvertUnitPerSecFramePerSec( rspeed1 );
	q.RotationAxis( rot_axis1, fps );
    qres = current_res * q;
    current_res = qres;
    

    fps = m_timemanager.ConvertUnitPerSecFramePerSec( rspeed2 );
	q.RotationAxis( rot_axis2, fps );
    qres = current_res * q;
    current_res = qres;



    current_res.RotationMatFrom( orientation );

    // resultat calcul est dans 'orientation'

    ////////////////////////////////////////

    orientation.Transform( &local_speed, &gs );
    m_timemanager.TranslationSpeedInc( &pos( 3, 0 ), gs[0] );
    m_timemanager.TranslationSpeedInc( &pos( 3, 1 ), gs[1] );
    m_timemanager.TranslationSpeedInc( &pos( 3, 2 ), gs[2] );

    p_out_base_transform = orientation * pos;

    mats[0]->getPurpose() = pos;
    quats[0]->getPurpose() = current_res;
    */


	Vector gs;
	m_orientation.Transform( &m_local_speed, &gs );

	dsreal x, y, z;
	x = m_position( 3, 0 );
	y = m_position( 3, 1 );
	z = m_position( 3, 2 );

	m_timemanager.TranslationSpeedInc( &x, gs[0] );
	m_timemanager.TranslationSpeedInc( &y, gs[1] );
	m_timemanager.TranslationSpeedInc( &z, gs[2] );

	m_position( 3, 0 ) = x;
	m_position( 3, 1 ) = y;
	m_position( 3, 2 ) = z;   

    m_result = m_orientation * m_position;

    p_out_base_transform = m_result;
}

