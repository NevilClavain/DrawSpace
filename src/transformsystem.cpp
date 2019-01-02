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

#include "transformsystem.h"
#include "transformaspect.h"
#include "cameraaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Systems;

TransformSystem::TransformSystem( void )
{
}

TransformSystem::~TransformSystem( void )
{
}


void TransformSystem::run( EntityNodeGraph* p_entitygraph )
{
    m_step = 0;
    p_entitygraph->AcceptSystemRootToLeaf( this );

    m_viewtransform_todispatch.Identity();
    m_projtransform_todispatch.Identity();

    Entity* curr_entity_camera = p_entitygraph->GetCurrentCameraEntity();

    if( curr_entity_camera )
    {
        CameraAspect* camera_aspect = curr_entity_camera->GetAspect<CameraAspect>();
        if( camera_aspect )
        {
            // chercher si worldaspect associe a cette camera
            // si oui, inverser cette matrice, qui servira de view
            // si non, identite pour view

            TransformAspect* transform_aspect = curr_entity_camera->GetAspect<TransformAspect>();
            if( transform_aspect )
            {
                Matrix camera_world_transform;
                transform_aspect->GetWorldTransform( camera_world_transform );
                camera_world_transform.Inverse();

                m_viewtransform_todispatch = camera_world_transform;
            }

            // chercher si composant Mat associe a camera_aspect
            // si oui, servira de proj
            // si non, identite pour proj

            ComponentList<Matrix> mats;
            camera_aspect->GetComponentsByType<Matrix>( mats );
            if( mats.size() > 0 )
            {
                m_projtransform_todispatch = mats[0]->getPurpose();
            }
        }
    }

    m_step = 1;
    p_entitygraph->AcceptSystemRootToLeaf( this );  
}

void TransformSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{
    if( 0 == m_step )
    {
        //calculer les matrices world pour tt le monde
        TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
        if( transform_aspect )
        {
            transform_aspect->ComputeTransforms( p_parent, p_entity );
        }
    }
    else if( 1 == m_step )
    {
        // distribuer view et proj a tout les transform_aspect de ttes les entites
        
        TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
        if( transform_aspect )
        {
            transform_aspect->DispatchViewProj( m_viewtransform_todispatch, m_projtransform_todispatch );
        }
    }
}
