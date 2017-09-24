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

#include "worldsystem.h"
#include "worldaspect.h"
#include "cameraaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Systems;

WorldSystem::WorldSystem( void ) :
m_curr_entity_camera( NULL )
{
}

WorldSystem::~WorldSystem( void )
{
}

void WorldSystem::Run( EntityNodeGraph* p_entitygraph )
{
    m_step = 0;
    p_entitygraph->AcceptWorldSystem( this );

    if( m_curr_entity_camera )
    {
        CameraAspect* camera_aspect = m_curr_entity_camera->GetAspect<CameraAspect>();
        if( camera_aspect )
        {
            /*
            Matrix view;
            Matrix proj;
            camera_aspect->GetProjTransform( proj );
            camera_aspect->GetViewTransform( view );
            */

            // chercher si worldaspect associe a cette camera
            // si oui, inverser cette matrice, qui servira de view
            // si non, identite pour view

            WorldAspect* world_aspect = m_curr_entity_camera->GetAspect<WorldAspect>();
            if( world_aspect )
            {
                Matrix camera_world_transform;
                world_aspect->GetWorldTransform( camera_world_transform );
                camera_world_transform.Inverse();
            }

            // chercher si composant Mat associe a camera_aspect
            // si oui, servira de proj
            // si on, identite pour proj
        }
    }

    m_step = 1;
    p_entitygraph->AcceptWorldSystem( this );
}

void WorldSystem::VisitEntity( Entity* p_parent, Entity* p_entity )
{
    if( 0 == m_step )
    {
        //calculer les matrices world pour tt le monde
        WorldAspect* world_aspect = p_entity->GetAspect<WorldAspect>();
        if( world_aspect )
        {
            world_aspect->ComputeTransforms( p_parent, p_entity );
        }
    }
    else if( 1 == m_step )
    {
        // distribuer view et proj a tout les world_aspect de ttes les entites
        // sauf l'entite correspondant a la camera            
    }
}

void WorldSystem::SetCurrentCameraEntity( Core::Entity* p_curr_entity_camera )
{
    m_curr_entity_camera = p_curr_entity_camera;
}
