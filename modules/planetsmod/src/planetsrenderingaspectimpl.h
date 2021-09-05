/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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
#include "timemanager.h"

#include "lod_config.h"
#include "lod_drawing.h"

#include "collisionaspect.h"

class PlanetDetailsBinder;
class PlanetClimateBinder;
class MultiFractalBinder;

namespace LOD
{
class Layer;
}

namespace DrawSpace
{
namespace Aspect
{
class CollisionAspect;
class TransformAspect;
}
namespace Systems
{
class Hub;
}
namespace AspectImplementations
{
class RigidBodyTransformAspectImpl;


class PlanetsRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{
protected:

    using SystemsEvtCb              = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, DrawSpace::Interface::System::Event, dsstring>;
    using CameraEventsCb            = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent, Core::Entity*>;
    using NodesEventsCb             = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, EntityGraph::EntityNode::Event, Core::Entity*>;
    using SubPassCreationCb         = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, LOD::SubPass::EntryInfos, LOD::SubPass*, LOD::SubPass::Destination>;
    using CollisionMesheUpdateCb    = DrawSpace::Core::CallBack3<PlanetsRenderingAspectImpl, void, dsstring, DrawSpace::Aspect::CollisionAspect::MesheCollisionShape, bool>;
    using TimerCb                   = DrawSpace::Core::CallBack<PlanetsRenderingAspectImpl, void, DrawSpace::Utils::Timer*>;

    using ViewOutInfos              = std::map<dsstring, std::tuple<int, bool, dsreal, dsreal, dsreal, dsreal, dsreal, DrawSpace::Utils::Vector>>;

    static const int DetailsLayer       = 0;
    static const int AtmosphereLayer    = 1;
    static const int FlatCloudsLayer    = 2;
    static const int OceansLayer        = 3;

    static const dsstring ClimateVShaderComponentName;
    static const dsstring ClimatePShaderComponentName;

    static const dsstring CollisionVShaderComponentName;
    static const dsstring CollisionPShaderComponentName;

    static const dsstring CollisionDisplayVShaderComponentName;
    static const dsstring CollisionDisplayPShaderComponentName;


protected:


    bool                                                m_add_in_rendergraph;
    Systems::Hub*                                       m_hub{ nullptr };
    SystemsEvtCb                                        m_system_evt_cb;
    CameraEventsCb                                      m_cameras_evt_cb;
    NodesEventsCb                                       m_nodes_evt_cb;
    EntityGraph::EntityNodeGraph*                       m_entitynodegraph{ nullptr };
    TimerCb                                             m_timer_cb;
    DrawSpace::Utils::Timer                             m_timer;
    DrawSpace::Utils::TimeManager*                      m_timemanager{ nullptr };;

    struct RegisteredCamera
    {
        Core::Entity*                       owner_entity; // l'entite possedant l'aspect camera
        dsstring                            camera_name;

        DrawSpace::Utils::Vector            locale_camera_pos_from_planet;

        bool                                relative_alt_valid;
        dsreal                              relative_alt;

        std::vector<LOD::Layer*>            layers;

        dsstring                            entity_name;

        DrawSpace::Aspect::TransformAspect* parent_body_transform_aspect;
    };

    struct RegisteredBody
    {
        RegisteredCamera*                   owner{ nullptr };
        DrawSpace::Aspect::TransformAspect* transform_aspect{ nullptr };
    };

    DrawSpace::Core::Entity*                                        m_owner_entity{ nullptr };

    dsreal                                                          m_planet_ray;
    dsreal                                                          m_gravity_acc{ 0.0 };

    LOD::SubPass::singleshot_subpasses_stack                        m_singleshot_subpasses_stack;
    LOD::SubPass::singleshot_subpasses                              m_singleshot_subpasses;
    LOD::SubPass::permanent_subpasses                               m_permanent_subpasses;

    std::vector<LOD::SubPass*>                                      m_m_permanent_subpasses_to_prepare;

    std::map<dsstring, RegisteredCamera>                            m_registered_camerapoints;

    RegisteredCamera*                                               m_current_camera{ nullptr };

    LOD::Config                                                     m_config;
    LOD::Drawing                                                    m_drawable;

    std::map<dsstring,std::array<PlanetDetailsBinder*, 6>>          m_planet_detail_binder;
    std::map<dsstring, std::array<PlanetDetailsBinder*, 6>>         m_planet_atmosphere_binder;
    std::map<dsstring, std::array<PlanetDetailsBinder*, 6>>         m_planet_flatclouds_binder;
    std::map<dsstring, std::array<PlanetDetailsBinder*, 6>>         m_planet_oceans_binder;


    std::array<PlanetClimateBinder*, 6>                             m_planet_climate_binder;
    std::array<MultiFractalBinder*, 6>                              m_planet_collision_binder;

    DrawSpace::Core::Fx                                             m_climate_fx;
    DrawSpace::Core::Shader*                                        m_climate_vshader{ nullptr };
    DrawSpace::Core::Shader*                                        m_climate_pshader{ nullptr };

    DrawSpace::Core::Fx                                             m_collisions_fx;
    DrawSpace::Core::Shader*                                        m_collisions_vshader{ nullptr };
    DrawSpace::Core::Shader*                                        m_collisions_pshader{ nullptr };

    DrawSpace::Core::Fx                                             m_collisions_display_fx;
    DrawSpace::Core::Shader*                                        m_collisions_display_vshader{ nullptr };
    DrawSpace::Core::Shader*                                        m_collisions_display_pshader{ nullptr };


    SubPassCreationCb                                               m_subpass_creation_cb;
    CollisionMesheUpdateCb                                          m_collisionmeshe_update_cb;

    std::vector<LOD::Layer*>                                        m_layers_list;

    DrawSpace::Interface::Renderer*                                 m_renderer;

    std::set<dsstring>                                              m_passes;

    bool                                                            m_enable_collisionmeshe_display{ false };


    bool                                                            m_collisions_active{ false };
    

    ///////////////////////////////////////////////////////////////////////////

    void                                        init_rendering_objects(void);
    void                                        release_rendering_objects(void);

    void                                        update_shader_params(void); // for all passes

    void                                        on_timer(DrawSpace::Utils::Timer* p_timer);
    void                                        on_system_event(DrawSpace::Interface::System::Event p_event, dsstring p_id);
    void                                        on_cameras_event(DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent p_event, Core::Entity* p_entity);
    void                                        on_nodes_event(DrawSpace::EntityGraph::EntityNode::Event p_event, Core::Entity* p_entity);
    LOD::SubPass::EntryInfos                    on_subpasscreation(LOD::SubPass* p_pass, LOD::SubPass::Destination p_dest);
    void                                        on_collisionmeshe_update(dsstring component_name, DrawSpace::Aspect::CollisionAspect::MesheCollisionShape p_shape, bool p_addcomponent);

    void                                        create_camera_collisions(PlanetsRenderingAspectImpl::RegisteredCamera& p_cameradescr, bool p_hotstate);

    void                                        draw_sub_passes(void);

    void                                        manage_gravity_targets(void);

    void                                        manage_camerapoints(void);

    void                                        details_control_from_viewer_alt(void);
    void                                        flatclouds_control_from_viewer_alt(void);
    void                                        oceans_control_from_viewer_alt(void);

    void                                        prepare_permanent_subpasses(void);

    void                                        setup_collisions_aspect(void);
    void                                        release_collisions_aspect(void);

    std::vector<RegisteredBody>                 get_bodies_list_from_registered_cams();
   
public:
    PlanetsRenderingAspectImpl( void );
    virtual ~PlanetsRenderingAspectImpl(void);

    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );

    void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );

    bool Init( DrawSpace::Core::Entity* p_entity, DrawSpace::Utils::TimeManager* p_timemanager );
    void Release(void);
    void Run( DrawSpace::Core::Entity* p_entity );
    void SetEntityNodeGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph);


    void ComponentsUpdated(void);

    virtual void SetHub(Systems::Hub* p_hub);
};
}
}

