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


#include "planetsrenderingaspectimpl.h"
#include "renderingaspect.h"
#include "transformaspect.h"
#include "cameraaspect.h"
#include "bodyaspect.h"
#include "informationsaspect.h"
#include "maths.h"
#include "hub.h"
#include <functional>


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Utils;


//Root::on_camera_event : 
// -> EntityNodeGraph::CAMERA_ACTIVE
// -> EntityNodeGraph::CAMERA_INACTIVE

//Root::on_nodes_event :
// -> EntityNode::ADDED_IN_TREE
// -> EntityNode::REMOVED_FROM_TREE

//Root::on_scenegraph_event :
// -> Systems::Hub::SYSTEMS_UPDATE_BEGIN
// -> Systems::Hub::SYSTEMS_UPDATE_END


PlanetsRenderingAspectImpl::PlanetsRenderingAspectImpl( void ) :
m_hub(NULL),
m_system_evt_cb( this, &PlanetsRenderingAspectImpl::on_system_event ),
m_cameras_evt_cb( this, &PlanetsRenderingAspectImpl::on_cameras_event),
m_nodes_evt_cb( this, &PlanetsRenderingAspectImpl::on_nodes_event),
m_entitynodegraph(NULL)
{
}

bool PlanetsRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue )
{
    bool updated_queue = false;

    /*
    for( size_t i = 0; i < m_pass_slots.size(); i++ )
    {
        if( m_pass_slots[i]->m_pass_name == p_name )
        {
            if( m_add_in_rendergraph )
            {
                // ajout du renderingnode dans la renderingqueue  
                p_passqueue->Add( m_pass_slots[i]->m_rendering_node );
            }
            else
            {
                // suppression du renderingnode de la renderingqueue
                p_passqueue->Remove( m_pass_slots[i]->m_rendering_node );
            }
            updated_queue = true;
        }
    }
    */
    return updated_queue;
}

void PlanetsRenderingAspectImpl::RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    //init_rendering_objects();

    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void PlanetsRenderingAspectImpl::UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    //release_rendering_objects();

    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}

bool PlanetsRenderingAspectImpl::Init(DrawSpace::Core::Entity* p_entity)
{
    if( m_hub )
    {
        std::vector<DrawSpace::Interface::System*> systems = m_hub->GetSystems();
        for(auto& e : systems )
        {
            e->RegisterSystemEvtHandler(&m_system_evt_cb);
        }
    }
    else
    {
        _DSEXCEPTION( "hub not set !!" )
    }

    return true;
}

void PlanetsRenderingAspectImpl::Release(void)
{
    if( m_hub )
    {
        std::vector<DrawSpace::Interface::System*> systems = m_hub->GetSystems();
        for (auto& e : systems)
        {
            e->UnregisterSystemEvtHandler(&m_system_evt_cb);
        }
    }
    else
    {
        _DSEXCEPTION( "hub not set !!" )
    }
}

void PlanetsRenderingAspectImpl::Run( DrawSpace::Core::Entity* p_entity )
{
    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();

    if( transform_aspect )
    {
        Matrix world;        
        transform_aspect->GetWorldTransform( world );

        Matrix view;
        transform_aspect->GetViewTransform( view );

        Matrix proj;
        transform_aspect->GetProjTransform( proj ); 
        /*
        for( size_t i = 0; i < m_pass_slots.size(); i++ )
        {
            m_pass_slots[i]->m_world = world;
            m_pass_slots[i]->m_view = view;
            m_pass_slots[i]->m_proj = proj;
        }
        */
    }

    update_shader_params();

    ////////////////////////////////////////////////////////
}

void PlanetsRenderingAspectImpl::init_rendering_objects( void )
{
    update_shader_params();   
}

void PlanetsRenderingAspectImpl::release_rendering_objects( void )
{
    /*
    for( size_t i = 0; i < m_pass_slots.size(); i++ )
    {
        _DRAWSPACE_DELETE_( m_pass_slots[i] );
    }
    m_pass_slots.clear();
    */
}

void PlanetsRenderingAspectImpl::update_shader_params( void ) // for all passes
{
}

void PlanetsRenderingAspectImpl::SetHub(Systems::Hub* p_hub)
{
    m_hub = p_hub;
}

void PlanetsRenderingAspectImpl::on_system_event(DrawSpace::Interface::System::Event p_event, dsstring p_id)
{
    _asm nop
}

void PlanetsRenderingAspectImpl::on_cameras_event(DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent p_event, Core::Entity* p_entity)
{
    CameraAspect* curr_camera_aspect = p_entity->GetAspect<CameraAspect>();
    dsstring cam_name = curr_camera_aspect->GetComponent<dsstring>("camera_name")->getPurpose();

    _asm nop
}

void PlanetsRenderingAspectImpl::on_nodes_event(DrawSpace::EntityGraph::EntityNode::Event p_event, Core::Entity* p_entity)
{
    dsstring entity_name;
    InfosAspect* infos_aspect = p_entity->GetAspect<InfosAspect>();
    if( infos_aspect )
    {
        entity_name = infos_aspect->GetComponent<dsstring>( "entity_name" )->getPurpose();

        CameraAspect* camera_aspect = p_entity->GetAspect<CameraAspect>();
        BodyAspect* body_aspect = p_entity->GetAspect<BodyAspect>();

        if (DrawSpace::EntityGraph::EntityNode::ADDED_IN_TREE == p_event)
        {
            if (0 == m_entities.count(entity_name))
            {
                // enregistrer cette entity
                m_entities[entity_name] = p_entity;
            }
            else
            {
                _DSEXCEPTION("entities with same name not allowed")
            }

            if( body_aspect )
            {
                RegisteredBody reg_body;
            

                //...

                m_registered_bodies[p_entity] = reg_body;
            }

            if (camera_aspect)
            {
                RegisteredCamera reg_camera;
                dsstring camera_name = camera_aspect->GetComponent<dsstring>("camera_name")->getPurpose();                
                reg_camera.camera_name = camera_name;

                Component<dsstring>* referent_body = infos_aspect->GetComponent<dsstring>("referent_body");
                if( referent_body )
                {
                    reg_camera.type = INERTBODY_LINKED;

                    dsstring referent_body_name = referent_body->getPurpose();

                    if (m_entities.count(referent_body_name))
                    {
                        reg_camera.attached_body = m_entities.at(referent_body_name);
                    }
                    else
                    {
                        _DSEXCEPTION("camera referent body : cannot find entity with corresponding name")
                    }

                    m_registered_camerapoints[camera_name] = reg_camera;
                }
                else
                {
                    reg_camera.type = FREE;
                    reg_camera.attached_body = NULL;

                    m_registered_camerapoints[camera_name] = reg_camera;
                }
            }
        }
        else if (DrawSpace::EntityGraph::EntityNode::REMOVED_FROM_TREE == p_event)
        {
            if (m_entities.count(entity_name))
            {
                // desenregistrer cette entity
                m_entities.erase(entity_name);
            }
        }
    }
}

void PlanetsRenderingAspectImpl::SetEntityNodeGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph)
{   
    if(p_entitynodegraph)
    {
        p_entitynodegraph->RegisterCameraEvtHandler( &m_cameras_evt_cb );
        p_entitynodegraph->RegisterNodesEvtHandler( &m_nodes_evt_cb );
    }
    else // p_entitynodegraph == NULL -> we unregister from callback
    {
        m_entitynodegraph->UnregisterCameraEvtHandler( &m_cameras_evt_cb );
        m_entitynodegraph->UnregisterNodesEvtHandler( &m_nodes_evt_cb );
    }
    m_entitynodegraph = p_entitynodegraph;
}