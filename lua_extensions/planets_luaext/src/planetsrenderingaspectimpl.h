/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
class MultiFractalBinder;

namespace LOD
{
class Layer;
}

namespace DrawSpace
{
namespace Core
{
class Entity;
}
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
}
}

class PlanetsRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{
public:

    struct RegisteredCamera
    {
        DrawSpace::Core::Entity*            owner_entity; // l'entite possedant l'aspect camera
        dsstring                            camera_name;

        DrawSpace::Maths::Vector            locale_camera_pos_from_planet;
        DrawSpace::Maths::Vector            locale_camera_long_lat;

        DrawSpace::Maths::Vector            global_camera_pos_from_planet;

        bool                                relative_alt_valid;
        dsreal                              relative_alt;

        std::vector<LOD::Layer*>            layers;

        dsstring                            entity_name;

        DrawSpace::Aspect::TransformAspect* parent_body_transform_aspect;
    };

    struct RegisteredBody
    {
        RegisteredCamera* owner{ nullptr };
        DrawSpace::Aspect::TransformAspect* transform_aspect{ nullptr };
    };

protected:

    using SystemsEvtCb              = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, DrawSpace::Interface::System::Event, dsstring>;
    using CameraEventsCb            = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent, DrawSpace::Core::Entity*>;
    using NodesEventsCb             = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, DrawSpace::EntityGraph::EntityNode::Event, DrawSpace::Core::Entity*>;
    using SubPassCreationCb         = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, LOD::SubPass::EntryInfos, LOD::SubPass*, LOD::SubPass::Destination>;
    using CollisionMesheUpdateCb    = DrawSpace::Core::CallBack3<PlanetsRenderingAspectImpl, void, dsstring, DrawSpace::Aspect::CollisionAspect::MesheCollisionShape, bool>;
    using TimerCb                   = DrawSpace::Core::CallBack<PlanetsRenderingAspectImpl, void, DrawSpace::Timer*>;
    using RenderPassEventCb         = DrawSpace::Core::CallBack2<PlanetsRenderingAspectImpl, void, DrawSpace::RenderGraph::RenderPassNodeGraph::RenderPassEvent, const dsstring&>;

    using ViewOutInfos              = std::map<dsstring, std::tuple<int, bool, 
                                                                    dsreal, 
                                                                    dsreal, 
                                                                    dsreal, 
                                                                    dsreal, 
                                                                    dsreal, 
                                                                    DrawSpace::Maths::Vector,
                                                                    DrawSpace::Maths::Vector,
                                                                    DrawSpace::Maths::Vector,
                                                                    dsreal,dsreal>>;

    static const dsstring ClimateVShaderComponentName;
    static const dsstring ClimatePShaderComponentName;

    static const dsstring CollisionVShaderComponentName;
    static const dsstring CollisionPShaderComponentName;

    static const dsstring CollisionDisplayVShaderComponentName;
    static const dsstring CollisionDisplayPShaderComponentName;

    bool                                                            m_add_in_rendergraph;
    DrawSpace::Systems::Hub*                                        m_hub{ nullptr };
    SystemsEvtCb                                                    m_system_evt_cb;
    CameraEventsCb                                                  m_cameras_evt_cb;
    NodesEventsCb                                                   m_nodes_evt_cb;
    DrawSpace::EntityGraph::EntityNodeGraph*                        m_entitynodegraph{ nullptr };
    TimerCb                                                         m_timer_cb;
    RenderPassEventCb                                               m_render_evt_cb;
    DrawSpace::Timer                                                m_timer;
    DrawSpace::TimeManager*                                         m_timemanager{ nullptr };;

    DrawSpace::Core::Entity*                                        m_owner_entity{ nullptr };

    dsreal                                                          m_planet_ray;
    dsreal                                                          m_gravity_acc{ 0.0 };

    LOD::SubPass::singleshot_subpasses_stack                        m_singleshot_subpasses_stack;
    LOD::SubPass::singleshot_subpasses                              m_singleshot_subpasses;

    std::map<dsstring, RegisteredCamera>                            m_registered_camerapoints;

    RegisteredCamera*                                               m_current_camera{ nullptr };

    LOD::Config                                                     m_config;
    LOD::Drawing                                                    m_drawable;

    std::map<dsstring, std::array<LOD::Binder*, 6>>                 m_planet_detail_binder_2;
    std::map<dsstring, std::array<LOD::Binder*, 6>>                 m_planet_atmosphere_binder_2;
    std::map<dsstring, std::array<LOD::Binder*, 6>>                 m_planet_flatclouds_binder_2;
    std::map<dsstring, std::array<LOD::Binder*, 6>>                 m_planet_oceans_binder_2;
    std::array<LOD::Binder*, 6>                                     m_planet_climate_binder;

    //std::map<dsstring, LOD::Binder*>                                m_planet_foliage_binder;

    std::map<dsstring, std::vector<LOD::Binder*>>                   m_planet_foliage_binder;

    

    std::array<LOD::Binder*, 6>                                     m_heightmap_binder;

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

    dsstring                                                        m_main_pass;
    dsstring                                                        m_reflection_pass;
    dsstring                                                        m_bump_pass;
    dsstring                                                        m_oceanmask_pass;
    

    ///////////////////////////////////////////////////////////////////////////

    void                                        init_rendering_objects(void);
    void                                        release_rendering_objects(void);

    void                                        update_shader_params(void); // for all passes

    void                                        on_timer(DrawSpace::Timer* p_timer);
    void                                        on_system_event(DrawSpace::Interface::System::Event p_event, dsstring p_id);
    void                                        on_cameras_event(DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent p_event, DrawSpace::Core::Entity* p_entity);
    void                                        on_nodes_event(DrawSpace::EntityGraph::EntityNode::Event p_event, DrawSpace::Core::Entity* p_entity);
    LOD::SubPass::EntryInfos                    on_subpasscreation(LOD::SubPass* p_pass, LOD::SubPass::Destination p_dest);
    void                                        on_collisionmeshe_update(dsstring component_name, DrawSpace::Aspect::CollisionAspect::MesheCollisionShape p_shape, bool p_addcomponent);
    void                                        on_render_event(DrawSpace::RenderGraph::RenderPassNodeGraph::RenderPassEvent p_evt, const dsstring& p_pass);

    void                                        create_camera_collisions(PlanetsRenderingAspectImpl::RegisteredCamera& p_cameradescr, bool p_hotstate, int p_nbLODs, bool p_freecamera);

    void                                        draw_sub_passes(void);

    void                                        manage_gravity_targets(void);

    void                                        manage_camerapoints(void);

    void                                        details_control_from_viewer_alt(void);
    void                                        flatclouds_control_from_viewer_alt(void);
    void                                        oceans_control_from_viewer_alt(void);

    void                                        setup_collisions_aspect(void);
    void                                        release_collisions_aspect(void);

    std::vector<RegisteredBody>                 get_bodies_list_from_registered_cams();
    bool                                        is_registeredcam_from_body(const RegisteredCamera& p_camera) const;
   
public:
    PlanetsRenderingAspectImpl( void );
    virtual ~PlanetsRenderingAspectImpl(void);

    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );

    void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );

    bool Init( DrawSpace::Core::Entity* p_entity, DrawSpace::TimeManager* p_timemanager );
    void Release(void);
    void Run( DrawSpace::Core::Entity* p_entity );
    void SetEntityNodeGraph(DrawSpace::EntityGraph::EntityNodeGraph* p_entitynodegraph);

    void ResetCollisionMesheValidity(void);
    void ComponentsUpdated(void);
    virtual void SetHub(DrawSpace::Systems::Hub* p_hub);

    /// accessors
    std::map<dsstring, RegisteredCamera>                    GetRegisteredCameras(void) const;
    dsstring                                                GetReflectionPassId(void) const;
    std::map<dsstring, std::array<LOD::Binder*, 6>>         GetPlanetFlatCloudsBinder2(void) const;
    std::map<dsstring, std::array<LOD::Binder*, 6>>         GetPlanetAtmoBinder2(void) const;


};


