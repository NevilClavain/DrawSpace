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

void FreeTransformAspectImpl::GetLocaleTransform( WorldAspect* p_worldaspect, Utils::Matrix& p_out_base_transform )
{
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
    dsreal rspeed = reals[0]->getPurpose();

    // axe demandé
    Vector rot_axis = vectors[1]->getPurpose();

    // quaternion resultat courant
    ComponentList<Quaternion>   quats;
    p_worldaspect->GetComponentsByType<Quaternion>( quats );
    Quaternion	                current_res = quats[0]->getPurpose();


    
    Utils::Matrix			    orientation;
    Vector                      gs;

    ////////////////////////////////////////

    //if( rspeed > 0 )
    {
	    Quaternion q, qres;
	    dsreal fps = m_timemanager.ConvertUnitPerSecFramePerSec( rspeed );
	    q.RotationAxis( rot_axis, fps );
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

    } 
}