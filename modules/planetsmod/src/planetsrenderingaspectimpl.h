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

#pragma once


#include "renderingaspectimpl.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"
#include "hub.h"

#include "lod_config.h"
#include "lod_drawing.h"

class PlanetDetailsBinder;

namespace DrawSpace
{
namespace Systems
{
class Hub;
}
namespace AspectImplementations
{
class PlanetsRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{
protected:

    using SystemsEvtCb      = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, DrawSpace::Interface::System::Event, dsstring>;
    using CameraEventsCb    = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent, Core::Entity*>;
    using NodesEventsCb     = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, EntityGraph::EntityNode::Event, Core::Entity*>;


protected:

    ///////////////////////////////////////////////////////////////////////////

    bool                                                m_add_in_rendergraph;
    Systems::Hub*                                       m_hub;
    SystemsEvtCb                                        m_system_evt_cb;
    CameraEventsCb                                      m_cameras_evt_cb;
    NodesEventsCb                                       m_nodes_evt_cb;
    EntityGraph::EntityNodeGraph*                       m_entitynodegraph;

    ///////////////////////////////////////////////////////////////////////////


    enum CameraType
    {
        FREE,
        FREE_ON_PLANET,
        INERTBODY_LINKED,
    };

    struct RegisteredBody
    {
    
    };

    struct RegisteredCamera
    {
        dsstring                    camera_name;
        CameraType                  type;
        DrawSpace::Core::Entity*    attached_body;
    };

    std::map<DrawSpace::Core::Entity*, RegisteredBody>              m_registered_bodies;
    std::map<dsstring, RegisteredCamera>                            m_registered_camerapoints;


    std::map<dsstring, DrawSpace::Core::Entity*>                    m_entities;  // stockage de toutes les entities du graph

    LOD::Config                                                     m_config;
    LOD::Drawing                                                    m_drawable;

    PlanetDetailsBinder*                                            m_planet_detail_binder[6] = { NULL };

    ///////////////////////////////////////////////////////////////////////////

    void            init_rendering_objects(void);
    void            release_rendering_objects(void);

    void            update_shader_params(void); // for all passes

    void            on_system_event(DrawSpace::Interface::System::Event p_event, dsstring p_id);
    void            on_cameras_event(DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent p_event, Core::Entity* p_entity);
    void            on_nodes_event(DrawSpace::EntityGraph::EntityNode::Event p_event, Core::Entity* p_entity);
   
public:
    PlanetsRenderingAspectImpl( void );

    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );

    void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );

    bool Init( DrawSpace::Core::Entity* p_entity );
    void Release(void);
    void Run( DrawSpace::Core::Entity* p_entity );
    void SetEntityNodeGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph);

    virtual void SetHub(Systems::Hub* p_hub);
};
}
}

