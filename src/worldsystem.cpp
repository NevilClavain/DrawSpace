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

    m_viewtransform_todispatch.Identity();
    m_projtransform_todispatch.Identity();

    if( m_curr_entity_camera )
    {
        CameraAspect* camera_aspect = m_curr_entity_camera->GetAspect<CameraAspect>();
        if( camera_aspect )
        {
            // chercher si worldaspect associe a cette camera
            // si oui, inverser cette matrice, qui servira de view
            // si non, identite pour view

            WorldAspect* world_aspect = m_curr_entity_camera->GetAspect<WorldAspect>();
            if( world_aspect )
            {
                Matrix camera_world_transform;
                world_aspect->GetWorldTransform( camera_world_transform );
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
        
        WorldAspect* world_aspect = p_entity->GetAspect<WorldAspect>();
        if( world_aspect )
        {
            world_aspect->DispatchViewProj( m_viewtransform_todispatch, m_projtransform_todispatch );
        }
    }
}

void WorldSystem::SetCurrentCameraEntity( Core::Entity* p_curr_entity_camera )
{
    m_curr_entity_camera = p_curr_entity_camera;

    for( std::vector<CameraEventHandler*>::iterator it = m_cameraevt_handlers.begin(); it != m_cameraevt_handlers.end(); ++it )
    {
        ( **it )( ACTIVE, m_curr_entity_camera );
    }
}

void WorldSystem::RegisterCameraEvtHandler( CameraEventHandler* p_handler )
{
    m_cameraevt_handlers.push_back( p_handler );

    // annoncer la camera active au nouvel abonne de cet evt...
    (*p_handler)( ACTIVE, m_curr_entity_camera );

}

void WorldSystem::UnregisterCameraEvtHandler( CameraEventHandler* p_handler )
{
    for( auto it = m_cameraevt_handlers.begin(); it != m_cameraevt_handlers.end(); ++it )
    {
        if( (*it) == p_handler )
        {
            m_cameraevt_handlers.erase( it );
            break;
        }
    }
}