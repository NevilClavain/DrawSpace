/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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
#include "csts.h"
#include "renderingaspect.h"
#include "transformaspect.h"
#include "cameraaspect.h"

#include "informationsaspect.h"
#include "resourcesaspect.h"
#include "maths.h"
#include "hub.h"
#include "runnersystem.h"
#include "updatequeuetask.h"

#include "planetdetailsbinder.h"
#include "planetclimatebinder.h"
#include <functional>

#include "lod_layer.h"

#include "collisionaspect.h"
#include "rigidbodytransformaspectimpl.h"



using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Systems;
using namespace DrawSpace::EntityGraph;


const dsstring PlanetsRenderingAspectImpl::ClimateVShaderComponentName = "climate_vshader";
const dsstring PlanetsRenderingAspectImpl::ClimatePShaderComponentName = "climate_pshader";

const dsstring PlanetsRenderingAspectImpl::CollisionVShaderComponentName = "collision_vshader";
const dsstring PlanetsRenderingAspectImpl::CollisionPShaderComponentName = "collision_pshader";

const dsstring PlanetsRenderingAspectImpl::CollisionDisplayVShaderComponentName = "collision_display__vshader";
const dsstring PlanetsRenderingAspectImpl::CollisionDisplayPShaderComponentName = "collision_display_pshader";



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
m_entitynodegraph(NULL),
m_drawable(&m_config),
m_subpass_creation_cb(this, &PlanetsRenderingAspectImpl::on_subpasscreation),
m_collisionmeshe_update_cb(this, &PlanetsRenderingAspectImpl::on_collisionmeshe_update),
m_timer_cb(this, &PlanetsRenderingAspectImpl::on_timer),
m_render_evt_cb(this, &PlanetsRenderingAspectImpl::on_render_event)
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_drawable.SetRenderer(m_renderer);

    LOD::Body::BuildMeshes();   
}

PlanetsRenderingAspectImpl::~PlanetsRenderingAspectImpl(void)
{
    LOD::Body::DestroyMeshes();

    for (size_t i = 0; i < m_layers_list.size(); i++)
    {
        LOD::Body* slod_body = m_layers_list[i]->GetBody();
        _DRAWSPACE_DELETE_(slod_body);

        _DRAWSPACE_DELETE_(m_layers_list[i]);
    }
}

bool PlanetsRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue )
{
    bool updated_queue = false;

    if(m_passes.count( p_name ))
    {
        if (m_add_in_rendergraph)
        {
            m_drawable.AddInRendergraph(p_name, p_passqueue);
        }
        else
        {
            m_drawable.RemoveFromRendergraph(p_name, p_passqueue);
        }
        updated_queue = true;
    }
    return updated_queue;
}

void PlanetsRenderingAspectImpl::RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    init_rendering_objects();

    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );

    p_rendergraph.RegisterRenderPassEvtHandler(&m_render_evt_cb);
}

void PlanetsRenderingAspectImpl::UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    p_rendergraph.UnregisterRenderPassEvtHandler(&m_render_evt_cb);

    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );

    release_rendering_objects();
}

bool PlanetsRenderingAspectImpl::Init(DrawSpace::Core::Entity* p_entity, DrawSpace::Utils::TimeManager* p_timemanager)
{
    if( m_hub )
    {
        std::vector<DrawSpace::Interface::System*> systems = m_hub->GetSystems();
        for(auto& e : systems )
        {
            e->RegisterSystemEvtHandler(&m_system_evt_cb);
        }

        m_owner_entity = p_entity;
        setup_collisions_aspect();
    }
    else
    {
        _DSEXCEPTION( "hub not set !!" )
    }

    m_timemanager = p_timemanager;

    m_timer.SetHandler(&m_timer_cb);
    m_timer.SetPeriod(LOD::cst::timerPeriod);
    m_timemanager->RegisterTimer(&m_timer);

    m_timer.SetState(true);

    return true;
}

void PlanetsRenderingAspectImpl::Release(void)
{
    if( m_hub )
    {
        /////////////// release collisions stuff...
        
        for (auto& camera : m_registered_camerapoints)
        {
            for (auto& camera_layer : camera.second.layers)
            {
                camera_layer->RemoveCollider();
            }
        }
        

        release_collisions_aspect();
        //////////////////////////////////////

        std::vector<DrawSpace::Interface::System*> systems = m_hub->GetSystems();
        for (auto& e : systems)
        {
            e->UnregisterSystemEvtHandler(&m_system_evt_cb);
        }

        for (auto& e : m_planet_climate_binder)
        {
            _DRAWSPACE_DELETE_( e );
        }

        for (auto& e : m_planet_collision_binder)
        {
            _DRAWSPACE_DELETE_(e);
        }


        for (auto& e : m_planet_detail_binder)
        {
            for (auto& e2 : e.second)
            {
                PlanetDetailsBinder* binder = e2;
                _DRAWSPACE_DELETE_(binder);
            }
        }

        for (auto& e : m_planet_atmosphere_binder)
        {
            for (auto& e2 : e.second)
            {
                PlanetDetailsBinder* binder = e2;
                _DRAWSPACE_DELETE_(binder);
            }
        }

        for (auto& e : m_planet_flatclouds_binder)
        {
            for (auto& e2 : e.second)
            {
                PlanetDetailsBinder* binder = e2;
                _DRAWSPACE_DELETE_(binder);
            }
        }

        for (auto& e : m_planet_oceans_binder)
        {
            for (auto& e2 : e.second)
            {
                PlanetDetailsBinder* binder = e2;
                _DRAWSPACE_DELETE_(binder);
            }
        }


        if(m_climate_vshader)
        {
            _DRAWSPACE_DELETE_(m_climate_vshader);
        }

        if (m_climate_pshader)
        {
            _DRAWSPACE_DELETE_(m_climate_pshader);
        }

        if (m_collisions_vshader)
        {
            _DRAWSPACE_DELETE_(m_collisions_vshader);
        }

        if (m_collisions_pshader)
        {
            _DRAWSPACE_DELETE_(m_collisions_pshader);
        }

        if (m_collisions_display_vshader)
        {
            _DRAWSPACE_DELETE_(m_collisions_display_vshader);
        }

        if (m_collisions_display_pshader)
        {
            _DRAWSPACE_DELETE_(m_collisions_display_pshader);
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

        for (auto& e : m_planet_detail_binder)
        {
            for (auto& e2 : e.second)
            {
                PlanetDetailsBinder* binder = e2;

                e2->Update( world );
            }
        }

        for (auto& e : m_planet_atmosphere_binder)
        {
            for (auto& e2 : e.second)
            {
                PlanetDetailsBinder* binder = e2;

                e2->Update( world );
            }
        }

        for (auto& e : m_planet_flatclouds_binder)
        {
            for (auto& e2 : e.second)
            {
                PlanetDetailsBinder* binder = e2;

                e2->Update(world);
            }
        }

        for (auto& e : m_planet_oceans_binder)
        {
            for (auto& e2 : e.second)
            {
                PlanetDetailsBinder* binder = e2;

                e2->Update(world);
            }
        }
    }
    else
    {
        _DSEXCEPTION("Planet must have transform aspect!!!")
    }

    update_shader_params();

    ////////////////////////////////////////////////////////

    details_control_from_viewer_alt();
    flatclouds_control_from_viewer_alt();
    oceans_control_from_viewer_alt();

    ////////////////////////////////////////////////////////

    if (m_owner->GetComponent<bool>("resources_ready")->getPurpose())
    {
        // we can prepare permanent sub passes (UpdateOutputQueueNoOpt, FlipOutputQueues, DeclareReady) only if all resources has been loaded
        prepare_permanent_subpasses();

        draw_sub_passes();
    }
    
    ///// Update OUT parameters in specific config

    m_owner->GetComponent<int>("OUT_delayedSingleSubPassQueueSize")->getPurpose() = m_singleshot_subpasses_stack.size();

    ViewOutInfos registeredCameraInfos;
    for(auto& e : m_registered_camerapoints)
    {
        int currentLOD = e.second.layers[DetailsLayer]->GetCurrentLOD();
        bool relative = e.second.layers[DetailsLayer]->GetHotState();
        dsreal rel_alt = 0.0;
        rel_alt = e.second.relative_alt;
        dsreal altitude = e.second.layers[DetailsLayer]->GetBody()->GetHotPointAltitud();

        dsreal current_patch_max_height = e.second.layers[DetailsLayer]->GetCurrentPatchMaxHeight();
        dsreal current_patch_min_height = e.second.layers[DetailsLayer]->GetCurrentPatchMinHeight();
        dsreal current_patch_current_height = e.second.layers[DetailsLayer]->GetCurrentPatchCurrentHeight();

        Vector locale_camera_pos{ e.second.locale_camera_pos_from_planet };
        Vector longlat_pos{ e.second.locale_camera_long_lat };
        Vector global_camera_pos{ e.second.global_camera_pos_from_planet };

        registeredCameraInfos[e.first] = std::make_tuple(currentLOD, 
                                                            relative, 
                                                            rel_alt, 
                                                            altitude, 
                                                            current_patch_max_height, 
                                                            current_patch_min_height, 
                                                            current_patch_current_height, 
                                                            locale_camera_pos,
                                                            longlat_pos,
                                                            global_camera_pos);
    }

    m_owner->GetComponent<ViewOutInfos>("OUT_viewsInfos")->getPurpose() = registeredCameraInfos;
}

void PlanetsRenderingAspectImpl::ComponentsUpdated(void)
{
    bool enable_atmosphere { m_owner->GetComponent<bool>("enable_atmosphere")->getPurpose() };
    dsreal ocean_bump_factor{ m_owner->GetComponent<dsreal>("ocean_bump_factor")->getPurpose() };
  
    dsreal oceandetails_specularpower{ m_owner->GetComponent<dsreal>("oceandetails_specularpower")->getPurpose() };

    // setup lights...
    
    using Lights = std::tuple<bool, std::array<dsreal, 3>, std::array<dsreal, 3>>;
    std::vector<Lights> lights = m_owner->GetComponent<std::vector<Lights>>("lights")->getPurpose();

    if (4 != lights.size())
    {
        _DSEXCEPTION("Planet lights : 4 entry required")
    }

    Lights ambient = lights[0];
    Lights dir0 = lights[1];
    Lights dir1 = lights[2];
    Lights dir2 = lights[3];

    ///////////////////////////////////////////////////////////////////////////

    for (auto& e : m_planet_detail_binder)
    {
        for (auto& e2 : e.second)
        {
            PlanetDetailsBinder* binder = e2;

            binder->m_ambient = std::get<0>(ambient);
            binder->m_ambient_color[0] = std::get<1>(ambient)[0];
            binder->m_ambient_color[1] = std::get<1>(ambient)[1];
            binder->m_ambient_color[2] = std::get<1>(ambient)[2];

            binder->m_lights[0].m_enable = std::get<0>(dir0);
            binder->m_lights[0].m_color[0] = std::get<1>(dir0)[0];
            binder->m_lights[0].m_color[1] = std::get<1>(dir0)[1];
            binder->m_lights[0].m_color[2] = std::get<1>(dir0)[2];
            binder->m_lights[0].m_dir[0] = -std::get<2>(dir0)[0];
            binder->m_lights[0].m_dir[1] = -std::get<2>(dir0)[1];
            binder->m_lights[0].m_dir[2] = -std::get<2>(dir0)[2];

            binder->m_lights[1].m_enable = std::get<0>(dir1);
            binder->m_lights[1].m_color[0] = std::get<1>(dir1)[0];
            binder->m_lights[1].m_color[1] = std::get<1>(dir1)[1];
            binder->m_lights[1].m_color[2] = std::get<1>(dir1)[2];
            binder->m_lights[1].m_dir[0] = -std::get<2>(dir1)[0];
            binder->m_lights[1].m_dir[1] = -std::get<2>(dir1)[1];
            binder->m_lights[1].m_dir[2] = -std::get<2>(dir1)[2];

            binder->m_lights[2].m_enable = std::get<0>(dir2);
            binder->m_lights[2].m_color[0] = std::get<1>(dir2)[0];
            binder->m_lights[2].m_color[1] = std::get<1>(dir2)[1];
            binder->m_lights[2].m_color[2] = std::get<1>(dir2)[2];
            binder->m_lights[2].m_dir[0] = -std::get<2>(dir2)[0];
            binder->m_lights[2].m_dir[1] = -std::get<2>(dir2)[1];
            binder->m_lights[2].m_dir[2] = -std::get<2>(dir2)[2];

            binder->EnableAtmoRender(enable_atmosphere);
        }
    }

    for (auto& e : m_planet_atmosphere_binder)
    {
        for (auto& e2 : e.second)
        {
            PlanetDetailsBinder* binder = e2;

            binder->m_ambient = std::get<0>(ambient);
            binder->m_ambient_color[0] = std::get<1>(ambient)[0];
            binder->m_ambient_color[1] = std::get<1>(ambient)[1];
            binder->m_ambient_color[2] = std::get<1>(ambient)[2];

            binder->m_lights[0].m_enable = std::get<0>(dir0);
            binder->m_lights[0].m_color[0] = std::get<1>(dir0)[0];
            binder->m_lights[0].m_color[1] = std::get<1>(dir0)[1];
            binder->m_lights[0].m_color[2] = std::get<1>(dir0)[2];
            binder->m_lights[0].m_dir[0] = -std::get<2>(dir0)[0];
            binder->m_lights[0].m_dir[1] = -std::get<2>(dir0)[1];
            binder->m_lights[0].m_dir[2] = -std::get<2>(dir0)[2];

            binder->m_lights[1].m_enable = std::get<0>(dir1);
            binder->m_lights[1].m_color[0] = std::get<1>(dir1)[0];
            binder->m_lights[1].m_color[1] = std::get<1>(dir1)[1];
            binder->m_lights[1].m_color[2] = std::get<1>(dir1)[2];
            binder->m_lights[1].m_dir[0] = -std::get<2>(dir1)[0];
            binder->m_lights[1].m_dir[1] = -std::get<2>(dir1)[1];
            binder->m_lights[1].m_dir[2] = -std::get<2>(dir1)[2];

            binder->m_lights[2].m_enable = std::get<0>(dir2);
            binder->m_lights[2].m_color[0] = std::get<1>(dir2)[0];
            binder->m_lights[2].m_color[1] = std::get<1>(dir2)[1];
            binder->m_lights[2].m_color[2] = std::get<1>(dir2)[2];
            binder->m_lights[2].m_dir[0] = -std::get<2>(dir2)[0];
            binder->m_lights[2].m_dir[1] = -std::get<2>(dir2)[1];
            binder->m_lights[2].m_dir[2] = -std::get<2>(dir2)[2];
        }
    }

    m_drawable.SetLayerNodeDrawingState(AtmosphereLayer, enable_atmosphere);

    for (auto& e : m_planet_flatclouds_binder)
    {
        for (auto& e2 : e.second)
        {
            PlanetDetailsBinder* binder = e2;

            binder->m_ambient = std::get<0>(ambient);
            binder->m_ambient_color[0] = std::get<1>(ambient)[0];
            binder->m_ambient_color[1] = std::get<1>(ambient)[1];
            binder->m_ambient_color[2] = std::get<1>(ambient)[2];

            binder->m_lights[0].m_enable = std::get<0>(dir0);
            binder->m_lights[0].m_color[0] = std::get<1>(dir0)[0];
            binder->m_lights[0].m_color[1] = std::get<1>(dir0)[1];
            binder->m_lights[0].m_color[2] = std::get<1>(dir0)[2];
            binder->m_lights[0].m_dir[0] = -std::get<2>(dir0)[0];
            binder->m_lights[0].m_dir[1] = -std::get<2>(dir0)[1];
            binder->m_lights[0].m_dir[2] = -std::get<2>(dir0)[2];

            binder->m_lights[1].m_enable = std::get<0>(dir1);
            binder->m_lights[1].m_color[0] = std::get<1>(dir1)[0];
            binder->m_lights[1].m_color[1] = std::get<1>(dir1)[1];
            binder->m_lights[1].m_color[2] = std::get<1>(dir1)[2];
            binder->m_lights[1].m_dir[0] = -std::get<2>(dir1)[0];
            binder->m_lights[1].m_dir[1] = -std::get<2>(dir1)[1];
            binder->m_lights[1].m_dir[2] = -std::get<2>(dir1)[2];

            binder->m_lights[2].m_enable = std::get<0>(dir2);
            binder->m_lights[2].m_color[0] = std::get<1>(dir2)[0];
            binder->m_lights[2].m_color[1] = std::get<1>(dir2)[1];
            binder->m_lights[2].m_color[2] = std::get<1>(dir2)[2];
            binder->m_lights[2].m_dir[0] = -std::get<2>(dir2)[0];
            binder->m_lights[2].m_dir[1] = -std::get<2>(dir2)[1];
            binder->m_lights[2].m_dir[2] = -std::get<2>(dir2)[2];
        }
    }

    for (auto& e : m_planet_oceans_binder)
    {
        for (auto& e2 : e.second)
        {
            PlanetDetailsBinder* binder = e2;

            binder->m_ambient = std::get<0>(ambient);
            binder->m_ambient_color[0] = std::get<1>(ambient)[0];
            binder->m_ambient_color[1] = std::get<1>(ambient)[1];
            binder->m_ambient_color[2] = std::get<1>(ambient)[2];

            binder->m_lights[0].m_enable = std::get<0>(dir0);
            binder->m_lights[0].m_color[0] = std::get<1>(dir0)[0];
            binder->m_lights[0].m_color[1] = std::get<1>(dir0)[1];
            binder->m_lights[0].m_color[2] = std::get<1>(dir0)[2];
            binder->m_lights[0].m_dir[0] = -std::get<2>(dir0)[0];
            binder->m_lights[0].m_dir[1] = -std::get<2>(dir0)[1];
            binder->m_lights[0].m_dir[2] = -std::get<2>(dir0)[2];

            binder->m_lights[1].m_enable = std::get<0>(dir1);
            binder->m_lights[1].m_color[0] = std::get<1>(dir1)[0];
            binder->m_lights[1].m_color[1] = std::get<1>(dir1)[1];
            binder->m_lights[1].m_color[2] = std::get<1>(dir1)[2];
            binder->m_lights[1].m_dir[0] = -std::get<2>(dir1)[0];
            binder->m_lights[1].m_dir[1] = -std::get<2>(dir1)[1];
            binder->m_lights[1].m_dir[2] = -std::get<2>(dir1)[2];

            binder->m_lights[2].m_enable = std::get<0>(dir2);
            binder->m_lights[2].m_color[0] = std::get<1>(dir2)[0];
            binder->m_lights[2].m_color[1] = std::get<1>(dir2)[1];
            binder->m_lights[2].m_color[2] = std::get<1>(dir2)[2];
            binder->m_lights[2].m_dir[0] = -std::get<2>(dir2)[0];
            binder->m_lights[2].m_dir[1] = -std::get<2>(dir2)[1];
            binder->m_lights[2].m_dir[2] = -std::get<2>(dir2)[2];

            binder->SetWaterBumpFactor(ocean_bump_factor);
            binder->SetOceanDetailsSpecPower(oceandetails_specularpower);
        }
    }
}

void PlanetsRenderingAspectImpl::init_rendering_objects(void)
{
    //// retrieve specific config....

    dsstring shaders_path{ m_owner->GetComponent<dsstring>("resources_path")->getPurpose() };

    dsreal planet_ray { m_owner->GetComponent<dsreal>("planet_ray")->getPurpose() };
    dsreal atmo_thickness { m_owner->GetComponent<dsreal>("atmo_thickness")->getPurpose() };
    dsreal flatclouds_altitude{ m_owner->GetComponent<dsreal>("flatclouds_altitude")->getPurpose() };
    dsreal plains_amplitude { m_owner->GetComponent<dsreal>("plains_amplitude")->getPurpose() };
    dsreal mountains_amplitude { m_owner->GetComponent<dsreal>("mountains_amplitude")->getPurpose() };
    dsreal vertical_offset { m_owner->GetComponent<dsreal>("vertical_offset")->getPurpose() };
    dsreal mountains_offset { m_owner->GetComponent<dsreal>("mountains_offset")->getPurpose() };
    dsreal plains_seed1 { m_owner->GetComponent<dsreal>("plains_seed1")->getPurpose() };
    dsreal plains_seed2 { m_owner->GetComponent<dsreal>("plains_seed2")->getPurpose() };
    dsreal mix_seed1 { m_owner->GetComponent<dsreal>("mix_seed1")->getPurpose() };
    dsreal mix_seed2 { m_owner->GetComponent<dsreal>("mix_seed2")->getPurpose() };

    dsreal terrainbump_factor { m_owner->GetComponent<dsreal>("terrainbump_factor")->getPurpose() };
    dsreal splat_transition_up_relative_alt { m_owner->GetComponent<dsreal>("splat_transition_up_relative_alt")->getPurpose() };
    dsreal splat_transition_down_relative_alt { m_owner->GetComponent<dsreal>("splat_transition_down_relative_alt")->getPurpose() };
    int splat_texture_resol { m_owner->GetComponent<int>("splat_texture_resol")->getPurpose() };

    dsreal atmo_kr { m_owner->GetComponent<dsreal>("atmo_kr")->getPurpose() };
    dsreal fog_alt_limit { m_owner->GetComponent<dsreal>("fog_alt_limit")->getPurpose() };
    dsreal fog_density { m_owner->GetComponent<dsreal>("fog_density")->getPurpose() };
    dsreal beach_limit { m_owner->GetComponent<dsreal>("beach_limit")->getPurpose() };

    bool enable_landplace_patch { m_owner->GetComponent<bool>("enable_landplace_patch")->getPurpose() };
    bool enable_atmosphere { m_owner->GetComponent<bool>("enable_atmosphere")->getPurpose() };

    int wave_pass_resol { m_owner->GetComponent<int>("wave_pass_resol")->getPurpose() };
    dsreal ocean_bump_factor{ m_owner->GetComponent<dsreal>("ocean_bump_factor")->getPurpose() };


    m_main_pass = m_owner->GetComponent<dsstring>("main_pass")->getPurpose();
    m_reflection_pass = m_owner->GetComponent<dsstring>("reflection_pass")->getPurpose();
    m_bump_pass = m_owner->GetComponent<dsstring>("bump_pass")->getPurpose();
    m_oceanmask_pass = m_owner->GetComponent<dsstring>("oceanmask_pass")->getPurpose();

    dsstring climate_vshader { m_owner->GetComponent<std::pair<dsstring, dsstring>>("climate_shaders")->getPurpose().first };
    dsstring climate_pshader { m_owner->GetComponent<std::pair<dsstring, dsstring>>("climate_shaders")->getPurpose().second };

    bool climate_vshader_compiled { m_owner->GetComponent<std::pair<bool, bool>>("climate_shaders_compiled")->getPurpose().first };
    bool climate_pshader_compiled { m_owner->GetComponent<std::pair<bool, bool>>("climate_shaders_compiled")->getPurpose().second };

    dsstring collision_vshader{ m_owner->GetComponent<std::pair<dsstring, dsstring>>("collisions_shaders")->getPurpose().first };
    dsstring collision_pshader{ m_owner->GetComponent<std::pair<dsstring, dsstring>>("collisions_shaders")->getPurpose().second };

    bool collision_vshader_compiled{ m_owner->GetComponent<std::pair<bool, bool>>("collisions_shaders_compiled")->getPurpose().first };
    bool collision_pshader_compiled{ m_owner->GetComponent<std::pair<bool, bool>>("collisions_shaders_compiled")->getPurpose().second };

    m_enable_collisionmeshe_display = m_owner->GetComponent<bool>("enable_collisionmeshe_display")->getPurpose();

    dsstring collisionmeshe_display_vshader{ m_owner->GetComponent<std::pair<dsstring, dsstring>>("collisionmeshe_display_shaders")->getPurpose().first };
    dsstring collisionmeshe_display_pshader{ m_owner->GetComponent<std::pair<dsstring, dsstring>>("collisionmeshe_display_shaders")->getPurpose().second };

    bool collisionmeshe_display_vshader_compiled{ m_owner->GetComponent<std::pair<bool, bool>>("collisionmeshe_display_shaders_compiled")->getPurpose().first };
    bool collisionmeshe_display_pshader_compiled{ m_owner->GetComponent<std::pair<bool, bool>>("collisionmeshe_display_shaders_compiled")->getPurpose().second };

    bool enable_oceans{ m_owner->GetComponent<bool>("oceans")->getPurpose() };

    dsreal oceandetails_specularpower{ m_owner->GetComponent<dsreal>("oceandetails_specularpower")->getPurpose() };

    Texture* wavepass_result_texture{ m_owner->GetComponent<Texture*>("wavepass_result_texture")->getPurpose() };


    using Lights = std::tuple<bool, std::array<dsreal, 3>, std::array<dsreal, 3>>;
    std::vector<Lights> lights = m_owner->GetComponent<std::vector<Lights>>("lights")->getPurpose();

    if( 4 != lights.size() )
    {
        _DSEXCEPTION("Planet lights : 4 entry required")
    }

    m_planet_ray = planet_ray * 1000.0;

    m_gravity_acc = m_owner->GetComponent<dsreal>("gravity_acc")->getPurpose();

    auto rcname_to_passes{ m_owner->GetComponent<std::map<dsstring, std::vector<dsstring>>>("rcname_to_passes")->getPurpose() };
    auto rcname_to_layer_index{ m_owner->GetComponent<std::map<dsstring, int>>("rcname_to_layer_index")->getPurpose() };

    auto layers_fx{ m_owner->GetComponent<std::vector<std::map<dsstring,Fx*>>>("layers_fx")->getPurpose() };
    auto layers_textures{ m_owner->GetComponent<std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("layers_textures")->getPurpose() };    
    auto layers_ro{ m_owner->GetComponent<std::vector<std::map<dsstring, int>>>("layers_ro")->getPurpose() };

        
    Shader::SetRootPath(shaders_path);

    m_climate_vshader = _DRAWSPACE_NEW_(Shader, Shader(climate_vshader, climate_vshader_compiled));
    m_climate_pshader = _DRAWSPACE_NEW_(Shader, Shader(climate_pshader, climate_pshader_compiled));

    m_collisions_vshader = _DRAWSPACE_NEW_(Shader, Shader(collision_vshader, collision_vshader_compiled));
    m_collisions_pshader = _DRAWSPACE_NEW_(Shader, Shader(collision_pshader, collision_pshader_compiled));

    if (m_enable_collisionmeshe_display)
    {
        m_collisions_display_vshader = _DRAWSPACE_NEW_(Shader, Shader(collisionmeshe_display_vshader, collisionmeshe_display_vshader_compiled));
        m_collisions_display_pshader = _DRAWSPACE_NEW_(Shader, Shader(collisionmeshe_display_pshader, collisionmeshe_display_pshader_compiled));
    }

    
    //////////// Resources ///////////////////////////

    Entity* owner_entity{ m_owner->GetOwnerEntity() };

    ResourcesAspect* resources_aspect = owner_entity->GetAspect<ResourcesAspect>();
    if (!resources_aspect)
    {
        _DSEXCEPTION("Planet : resources aspect required for planet entity")
    }

    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(ClimateVShaderComponentName, std::make_tuple(m_climate_vshader, false, 0));
    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(ClimatePShaderComponentName, std::make_tuple(m_climate_pshader, false, 1));

    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(CollisionVShaderComponentName, std::make_tuple(m_collisions_vshader, false, 0));
    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(CollisionPShaderComponentName, std::make_tuple(m_collisions_pshader, false, 1));

    if (m_enable_collisionmeshe_display)
    {
        resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(CollisionDisplayVShaderComponentName, std::make_tuple(m_collisions_display_vshader, false, 0));
        resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>(CollisionDisplayPShaderComponentName, std::make_tuple(m_collisions_display_pshader, false, 1));
    }


    /////////////////

    m_climate_fx.AddShader(m_climate_vshader);
    m_climate_fx.AddShader(m_climate_pshader);

    RenderStatesSet climate_rss;
    climate_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear"));
    climate_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none"));

    m_climate_fx.SetRenderStates(climate_rss);


    /////////////////

    m_collisions_fx.AddShader(m_collisions_vshader);
    m_collisions_fx.AddShader(m_collisions_pshader);

    RenderStatesSet collisions_rss;
    collisions_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear"));
    collisions_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear"));

    collisions_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none"));
    collisions_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none"));

    m_collisions_fx.SetRenderStates(collisions_rss);

    /////////////////
    if (m_enable_collisionmeshe_display)
    {
        m_collisions_display_fx.AddShader(m_collisions_display_vshader);
        m_collisions_display_fx.AddShader(m_collisions_display_pshader);

        RenderStatesSet collisions_display_rss;

        collisions_display_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETFILLMODE, "line"));
        collisions_display_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETFILLMODE, "solid"));

        m_collisions_display_fx.SetRenderStates(collisions_display_rss);
    }

    /////////////////

    // complete m_config layers

    m_config.m_landplace_patch = enable_landplace_patch;    
    m_config.m_lod0base = 19000.0;    
    m_config.m_nbLODRanges_inertBodies = 15;
    m_config.m_nbLODRanges_freeCameras = 14;

    for (auto& rcp : rcname_to_passes)
    {
        dsstring rendercontextname{ rcp.first };

        int layer{ rcname_to_layer_index[rendercontextname] };
        LOD::Config::LayerDescriptor ld;

        switch (layer)
        {
            case DetailsLayer:

                ld.enable_collisions = true;
                ld.enable_datatextures = true;
                ld.enable_lod = true;
                ld.min_lodlevel = 0;
                ld.ray = planet_ray;
                ld.description = "Details Layer";
                for (int i = 0; i < 6; i++)
                {
                    PlanetClimateBinder* climateBinder = _DRAWSPACE_NEW_(PlanetClimateBinder, PlanetClimateBinder(plains_amplitude, mountains_amplitude, vertical_offset, mountains_offset,
                        plains_seed1, plains_seed2, mix_seed1, mix_seed2, beach_limit, enable_oceans));

                    climateBinder->SetRenderer(m_renderer);
                    climateBinder->SetFx(&m_climate_fx);

                    MultiFractalBinder* collisionsBinder = _DRAWSPACE_NEW_(MultiFractalBinder, MultiFractalBinder(plains_amplitude, mountains_amplitude, vertical_offset, mountains_offset,
                        plains_seed1, plains_seed2, mix_seed1, mix_seed2));

                    collisionsBinder->SetRenderer(m_renderer);
                    collisionsBinder->SetFx(&m_collisions_fx);

                    ld.groundCollisionsBinder[i] = collisionsBinder;
                    ld.patchTexturesBinder[i] = climateBinder;

                    m_planet_climate_binder[i] = climateBinder;
                    m_planet_collision_binder[i] = collisionsBinder;
                }

                m_config.m_layers_descr[layer] = ld;
                break;

            case AtmosphereLayer:
                ld.enable_collisions = false;
                ld.enable_datatextures = false;
                ld.enable_lod = false;
                ld.min_lodlevel = 0;
                ld.ray = planet_ray + atmo_thickness;
                ld.description = "Atmosphere Layer";
                for (int i = 0; i < 6; i++)
                {
                    ld.groundCollisionsBinder[i] = NULL;
                    ld.patchTexturesBinder[i] = NULL;
                }

                m_config.m_layers_descr[layer] = ld;
                break;

            case FlatCloudsLayer:
                ld.enable_collisions = false;
                ld.enable_datatextures = false;
                ld.enable_lod = false;
                ld.min_lodlevel = 0;
                ld.ray = planet_ray + flatclouds_altitude;
                ld.description = "Flat clouds Layer";

                for (int i = 0; i < 6; i++)
                {
                    ld.groundCollisionsBinder[i] = NULL;
                    ld.patchTexturesBinder[i] = NULL;
                }

                m_config.m_layers_descr[layer] = ld;
                break;

            case OceansLayer:
                ld.enable_collisions = false;
                ld.enable_datatextures = false;
                ld.enable_lod = true;
                ld.min_lodlevel = 0;
                ld.ray = planet_ray;
                ld.description = "Oceans Layer";

                for (int i = 0; i < 6; i++)
                {
                    ld.groundCollisionsBinder[i] = NULL;
                    ld.patchTexturesBinder[i] = NULL;
                }

                m_config.m_layers_descr[layer] = ld;
                break;
        }


        auto    ros_map{ layers_ro[rcname_to_layer_index[rendercontextname]] };
        auto    fxs_map{ layers_fx[rcname_to_layer_index[rendercontextname]] };
        auto    textures_map{ layers_textures[rcname_to_layer_index[rendercontextname]] };

        int     ro{ ros_map.at(rendercontextname) };
        Fx*     fx{ fxs_map.at(rendercontextname) };
        auto    textures{ textures_map.at(rendercontextname) };

        std::array<Texture*, RenderingNode::NbMaxTextures> pass_textures;
        if (textures.size() > 0)
        {
            pass_textures = textures[0];
        }
       
        for (auto& pass_id : rcp.second)
        {
            m_passes.insert(pass_id);

            if (DetailsLayer == layer)
            {
                std::array<PlanetDetailsBinder*, 6> details_binders;

                for (int orientation = 0; orientation < 6; orientation++)
                {
                    PlanetDetailsBinder* binder = _DRAWSPACE_NEW_(PlanetDetailsBinder, PlanetDetailsBinder(planet_ray * 1000.0, atmo_thickness * 1000.0, plains_amplitude,
                        mountains_amplitude, vertical_offset, mountains_offset, plains_seed1, plains_seed2,
                        mix_seed1, mix_seed2, terrainbump_factor, splat_transition_up_relative_alt,
                        splat_transition_down_relative_alt, splat_texture_resol, atmo_kr,
                        fog_alt_limit, fog_density, enable_oceans, oceandetails_specularpower));

                    binder->SetFx(fx);
                    binder->SetRenderer(m_renderer);
                    for (size_t stage = 0; stage < pass_textures.size(); stage++)
                    {
                        binder->SetTexture(pass_textures[stage], stage);
                    }

                    binder->EnableAtmoRender(enable_atmosphere);

                    m_drawable.RegisterSinglePassSlot(pass_id, binder, orientation, LOD::Body::LOWRES_SKIRT_MESHE, DetailsLayer, ro);
                    details_binders[orientation] = binder;
                }

                m_planet_detail_binder[pass_id] = details_binders;

                if (m_enable_collisionmeshe_display)
                {
                    m_drawable.RegisterSinglePassSlotForCollisionDisplay(pass_id, &m_collisions_display_fx, ro + 1);
                }                
            }
            else if (AtmosphereLayer == layer)
            {
                std::array<PlanetDetailsBinder*, 6> atmo_binders;

                for (int orientation = 0; orientation < 6; orientation++)
                {
                    PlanetDetailsBinder* binder = _DRAWSPACE_NEW_(PlanetDetailsBinder, PlanetDetailsBinder(planet_ray * 1000.0, atmo_thickness * 1000.0, plains_amplitude,
                        mountains_amplitude, vertical_offset, mountains_offset, plains_seed1, plains_seed2,
                        mix_seed1, mix_seed2, terrainbump_factor, splat_transition_up_relative_alt,
                        splat_transition_down_relative_alt, splat_texture_resol, atmo_kr,
                        fog_alt_limit, fog_density, enable_oceans, oceandetails_specularpower));

                    binder->SetFx(fx);
                    binder->SetRenderer(m_renderer);

                    m_drawable.RegisterSinglePassSlot(pass_id, binder, orientation, LOD::Body::HIRES_MESHE, AtmosphereLayer, ro);
                    atmo_binders[orientation] = binder;
                }

                m_planet_atmosphere_binder[pass_id] = atmo_binders;
            }
            else if (FlatCloudsLayer == layer)
            {
                std::array<PlanetDetailsBinder*, 6> flatclouds_binders;

                for (int orientation = 0; orientation < 6; orientation++)
                {
                    PlanetDetailsBinder* binder = _DRAWSPACE_NEW_(PlanetDetailsBinder, PlanetDetailsBinder(planet_ray * 1000.0, atmo_thickness * 1000.0, plains_amplitude,
                        mountains_amplitude, vertical_offset, mountains_offset, plains_seed1, plains_seed2,
                        mix_seed1, mix_seed2, terrainbump_factor, splat_transition_up_relative_alt,
                        splat_transition_down_relative_alt, splat_texture_resol, atmo_kr,
                        fog_alt_limit, fog_density, enable_oceans, oceandetails_specularpower));

                    binder->SetFx(fx);
                    binder->SetRenderer(m_renderer);

                    for (size_t stage = 0; stage < pass_textures.size(); stage++)
                    {
                        binder->SetTexture(pass_textures[stage], stage);
                    }

                    m_drawable.RegisterSinglePassSlot(pass_id, binder, orientation, LOD::Body::AVGRES_MESHE, FlatCloudsLayer, ro);
                    flatclouds_binders[orientation] = binder;
                }

                m_planet_flatclouds_binder[pass_id] = flatclouds_binders;
            }
            else if (OceansLayer == layer)
            {
                std::array<PlanetDetailsBinder*, 6> oceans_binders;

                for (int orientation = 0; orientation < 6; orientation++)
                {
                    PlanetDetailsBinder* binder = _DRAWSPACE_NEW_(PlanetDetailsBinder, PlanetDetailsBinder(planet_ray * 1000.0, atmo_thickness * 1000.0, plains_amplitude,
                        mountains_amplitude, vertical_offset, mountains_offset, plains_seed1, plains_seed2,
                        mix_seed1, mix_seed2, terrainbump_factor, splat_transition_up_relative_alt,
                        splat_transition_down_relative_alt, splat_texture_resol, atmo_kr,
                        fog_alt_limit, fog_density, enable_oceans, oceandetails_specularpower));

                    binder->SetFx(fx);
                    binder->SetRenderer(m_renderer);
                    if (m_bump_pass == pass_id)
                    {
                        binder->SetTexture(wavepass_result_texture, 0);
                        binder->SetWaveTextureResol(wave_pass_resol);
                        binder->SetWaterBumpFactor(ocean_bump_factor);

                        m_drawable.RegisterSinglePassSlot(pass_id, binder, orientation, LOD::Body::LOWRES_MESHE, OceansLayer, ro, 1);
                    }
                    else
                    {
                        m_drawable.RegisterSinglePassSlot(pass_id, binder, orientation, LOD::Body::LOWRES_SKIRT_MESHE, OceansLayer, ro);
                    }                    
                    oceans_binders[orientation] = binder;
                }

                m_planet_oceans_binder[pass_id] = oceans_binders;
            }
        }
    }

    ComponentsUpdated();
    
    m_drawable.Startup( m_owner->GetOwnerEntity() );

    update_shader_params();
}

void PlanetsRenderingAspectImpl::release_rendering_objects( void )
{

    ResourcesAspect* resources_aspect = m_owner->GetOwnerEntity()->GetAspect<ResourcesAspect>();

    resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(ClimateVShaderComponentName);
    resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(ClimatePShaderComponentName);

    resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(CollisionVShaderComponentName);
    resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(CollisionPShaderComponentName);


    if (m_enable_collisionmeshe_display)
    {
        resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(CollisionDisplayVShaderComponentName);
        resources_aspect->RemoveComponent<std::tuple<Shader*, bool, int>>(CollisionDisplayPShaderComponentName);
    }
    m_drawable.Shutdown();
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
    if (m_owner->GetComponent<bool>("resources_ready")->getPurpose())
    {
        if ("TransformSystem" == p_id)
        {
            if (DrawSpace::Interface::System::Event::SYSTEM_RUN_BEGIN == p_event)
            {
                // apply gravity
                manage_gravity_targets();
            }
            else if (DrawSpace::Interface::System::Event::SYSTEM_RUN_END == p_event)
            {
                //manage_bodies();
                manage_camerapoints();
            }
        }
    }
}

void PlanetsRenderingAspectImpl::on_cameras_event(DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent p_event, Core::Entity* p_entity)
{
    if (!p_entity)
    {
        return;
    }
    CameraAspect* curr_camera_aspect = p_entity->GetAspect<CameraAspect>();
    dsstring cam_name = curr_camera_aspect->GetComponent<dsstring>("camera_name")->getPurpose();

    if(DrawSpace::EntityGraph::EntityNodeGraph::CAMERA_ACTIVE == p_event)
    {
        if (m_registered_camerapoints.count(cam_name) > 0)
        {
            m_current_camera = &m_registered_camerapoints.at(cam_name);

            std::vector<LOD::Body*> planet_bodies;

            for (size_t i = 0; i < m_registered_camerapoints[cam_name].layers.size(); i++)
            {
                LOD::Layer* layer = m_registered_camerapoints[cam_name].layers[i];

                planet_bodies.push_back(layer->GetBody());
            }
            m_drawable.SetCurrentPlanetBodies(planet_bodies);
        }
    }
}

void PlanetsRenderingAspectImpl::on_nodes_event(DrawSpace::EntityGraph::EntityNode::Event p_event, Core::Entity* p_entity)
{
    InfosAspect* infos_aspect = p_entity->GetAspect<InfosAspect>();
    if( infos_aspect )
    {
        dsstring entity_name                        { infos_aspect->GetComponent<dsstring>("entity_name")->getPurpose() };
        CameraAspect* camera_aspect                 { p_entity->GetAspect<CameraAspect>() };
        TransformAspect* transformation_aspect      { p_entity->GetAspect<TransformAspect>() };
       
        if (DrawSpace::EntityGraph::EntityNode::ADDED_IN_TREE == p_event)
        {
            if (camera_aspect)
            {
                RegisteredCamera reg_camera;

                reg_camera.entity_name = entity_name;
                dsstring camera_name = camera_aspect->GetComponent<dsstring>("camera_name")->getPurpose();                
                reg_camera.camera_name = camera_name;
                reg_camera.relative_alt_valid = false;

                reg_camera.owner_entity = p_entity;
                reg_camera.parent_body_transform_aspect = nullptr;

                create_camera_collisions(reg_camera, false);

                m_registered_camerapoints[camera_name] = reg_camera;

                //////////////////////////////////////////////////////////////////////////////////////////
            }
        }
        else // REMOVED_FROM_TREE
        {
            if (camera_aspect)
            {
                dsstring camera_name = camera_aspect->GetComponent<dsstring>("camera_name")->getPurpose();

                if (m_registered_camerapoints.count(camera_name))
                {
                    RegisteredCamera reg_camera{ m_registered_camerapoints.at(camera_name)};

                    if (reg_camera.parent_body_transform_aspect)
                    {
                        dsstring gravity_force_comp_name{ dsstring("planet_gravity_") + std::to_string((int)this) };
                        if (reg_camera.parent_body_transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Force>(gravity_force_comp_name))
                        {
                            reg_camera.parent_body_transform_aspect->RemoveComponent<RigidBodyTransformAspectImpl::Force>(gravity_force_comp_name);
                        }
                    }
                }
            }
        }
    }
}

void PlanetsRenderingAspectImpl::on_timer(DrawSpace::Utils::Timer* p_timer)
{
    if (m_singleshot_subpasses_stack.size() > 0)
    {
        LOD::SubPass* pass = m_singleshot_subpasses_stack.back();
        pass->SetTimerReadyFlag(true);
    }
}

void PlanetsRenderingAspectImpl::draw_sub_passes(void)
{
    RunnerSystem& runner_system{ m_hub->GetSystem<RunnerSystem>("RunnerSystem") };

    if (m_singleshot_subpasses_stack.size() > 0)
    {
        LOD::SubPass* sp = m_singleshot_subpasses_stack.back();
        if (sp->GetTimerReadyFlag())
        {
            m_singleshot_subpasses_stack.pop_back();

            RenderingQueue* rendering_queue{ sp->GetPass()->GetRenderingQueue() };            
            rendering_queue->UpdateOutputQueueNoOpt();
            rendering_queue->FlipOutputQueues();
            rendering_queue->DeclareReady();
            
            sp->DrawSubPass();
            sp->SubPassDone();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (m_singleshot_subpasses.size() > 0)
    {
        RunnerSequenceStep singleshot_subpasses_sequence_step;

        dsstring singleshot_subpasses_sequence_id{ std::to_string(GetTickCount()) + dsstring("/") };

        for (auto& e : m_singleshot_subpasses)
        {
            LOD::SubPass* sp = e;
            RenderingQueue* rendering_queue{ sp->GetPass()->GetRenderingQueue() };

            singleshot_subpasses_sequence_id = singleshot_subpasses_sequence_id + rendering_queue->GetId() + dsstring("/");
        }
        
        singleshot_subpasses_sequence_step.AddComponent<LOD::SubPass::singleshot_subpasses>("singleshot_subpasses_queues", m_singleshot_subpasses);
        singleshot_subpasses_sequence_step.AddComponent<dsstring>("sequence_id", singleshot_subpasses_sequence_id);

        m_singleshot_subpasses.clear();

        singleshot_subpasses_sequence_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
        {
            auto singleshot_subpasses{ p_step.GetComponent<LOD::SubPass::singleshot_subpasses>("singleshot_subpasses_queues")->getPurpose() };
            auto sequence_id{ p_step.GetComponent<dsstring>("sequence_id")->getPurpose() };

            UpdateQueueTask* task{ _DRAWSPACE_NEW_(UpdateQueueTask, UpdateQueueTask) };

            std::vector<DrawSpace::Core::RenderingQueue*> queues;

            for (auto& e : singleshot_subpasses)
            {
                LOD::SubPass* sp = e;
                RenderingQueue* rendering_queue{ sp->GetPass()->GetRenderingQueue() };

                queues.push_back(rendering_queue);
            }

            task->SetTargetDescr(sequence_id);
            task->SetActionDescr("UPDATE_SINGLESHOT_SUBPASS_QUEUES");
            task->SetRenderingQueue(queues);
            task->DisableOpt(true);

            p_step.SetTask(task);
        });


        singleshot_subpasses_sequence_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
        {
            auto singleshot_subpasses{ p_step.GetComponent<LOD::SubPass::singleshot_subpasses>("singleshot_subpasses_queues")->getPurpose() };

            for (auto& e : singleshot_subpasses)
            {
                LOD::SubPass* sp = e;
                RenderingQueue* rendering_queue{ sp->GetPass()->GetRenderingQueue() };

                rendering_queue->FlipOutputQueues();
                rendering_queue->DeclareReady();

                sp->DrawSubPass();
                sp->SubPassDone();
            }

            UpdateQueueTask* task{ static_cast<UpdateQueueTask*>(p_step.GetTask()) };

            _DRAWSPACE_DELETE_(task);
            p_seq.DeclareCompleted();

            p_step.RemoveComponent<LOD::SubPass::singleshot_subpasses>("singleshot_subpasses_queues");
            p_step.RemoveComponent<dsstring>("sequence_id");
        });

        RunnerSequence singleshot_subpasses_sequence;
        singleshot_subpasses_sequence.RegisterStep(dsstring("singleshot_subpasses_sequence_step"), singleshot_subpasses_sequence_step);
        singleshot_subpasses_sequence.SetCurrentStep(dsstring("singleshot_subpasses_sequence_step"));
        runner_system.RegisterSequence(singleshot_subpasses_sequence_id, singleshot_subpasses_sequence);
    }

    for (size_t i = 0; i < m_permanent_subpasses.size(); i++)
    {
        LOD::SubPass* sp{ m_permanent_subpasses[i] };
        sp->DrawSubPass();
        sp->SubPassDone();
    }
}

void PlanetsRenderingAspectImpl::prepare_permanent_subpasses(void)
{
    for (auto& e : m_m_permanent_subpasses_to_prepare)
    {
        LOD::SubPass* sp = e;

        sp->GetPass()->GetRenderingQueue()->UpdateOutputQueueNoOpt();
        sp->GetPass()->GetRenderingQueue()->FlipOutputQueues();
        sp->GetPass()->GetRenderingQueue()->DeclareReady();
    }
    m_m_permanent_subpasses_to_prepare.clear();
}

void PlanetsRenderingAspectImpl::on_collisionmeshe_update(dsstring component_name, DrawSpace::Aspect::CollisionAspect::MesheCollisionShape p_shape, bool p_addcomponent)
{
    if (m_collisions_active)
    {
        m_entitynodegraph->UnregisterCollider(m_owner_entity);
    }
    
    DrawSpace::Aspect::CollisionAspect* collision_aspect{ m_owner_entity->GetAspect<CollisionAspect>() };
    if (p_addcomponent)
    {
        if (!collision_aspect->GetComponent<CollisionAspect::MesheCollisionShape>(component_name))
        {
            collision_aspect->AddComponent<CollisionAspect::MesheCollisionShape>(component_name, p_shape);
        }
    }
    else
    {
        if (collision_aspect->GetComponent<CollisionAspect::MesheCollisionShape>(component_name))
        {
            collision_aspect->RemoveComponent<CollisionAspect::MesheCollisionShape>(component_name);
        }
    }

    // check if there is still CollisionAspect::MesheCollisionShape registered
    ComponentList<CollisionAspect::MesheCollisionShape> meshecollision_shapes;
    collision_aspect->GetComponentsByType<CollisionAspect::MesheCollisionShape>(meshecollision_shapes);
    if (meshecollision_shapes.size() > 0) 
    {
        m_entitynodegraph->RegisterCollider(m_owner_entity);
        m_collisions_active = true;
    }
    else
    {
        // no more CollisionAspect::MesheCollisionShape registered
        m_collisions_active = false;
    }
}

LOD::SubPass::EntryInfos PlanetsRenderingAspectImpl::on_subpasscreation(LOD::SubPass* p_pass, LOD::SubPass::Destination p_dest)
{
    LOD::FaceDrawingNode* node = static_cast<LOD::FaceDrawingNode*>(p_pass->GetNode());
    node->RegisterHandler(m_drawable.GetSingleNodeDrawHandler());
    // on ajoute le node a la queue directement ici
    p_pass->GetPass()->GetRenderingQueue()->Add(node);

    
    if (LOD::SubPass::PERMANENT_SUBPASS == p_dest)
    {        
        m_m_permanent_subpasses_to_prepare.push_back(p_pass);
    }
    
    ResourcesAspect* resources_aspect = m_owner->GetOwnerEntity()->GetAspect<ResourcesAspect>();
    if (!resources_aspect)
    {
        _DSEXCEPTION("Planet : resources aspect required for planet entity")
    }

    LOD::SubPass::EntryInfos ei;
    
    ei.singleshot_subpasses_stack = &m_singleshot_subpasses_stack;
    ei.singleshot_subpasses = &m_singleshot_subpasses;
    ei.permanent_subpasses = &m_permanent_subpasses;

    switch (p_dest)
    {
        case LOD::SubPass::DELAYED_SINGLE_SUBPASS:
            m_singleshot_subpasses_stack.push_front(p_pass);
            ei.singleshot_subpasses_stack_position = m_singleshot_subpasses_stack.begin();
            p_pass->SetTimerReadyFlag( false );
            break;

        case LOD::SubPass::IMMEDIATE_SINGLE_SUBPASS:
            m_singleshot_subpasses.push_back(p_pass);
            ei.singleshot_subpasses_position = m_singleshot_subpasses.end();
            --ei.singleshot_subpasses_position;
            break;

        case LOD::SubPass::PERMANENT_SUBPASS:
            m_permanent_subpasses.push_back(p_pass);
            ei.permanent_subpasses_position = m_permanent_subpasses.end() - 1;
            break;

        default:
            _DSEXCEPTION("unknow destination for subpass");
            break;
    }
    ei.queue_id = p_dest;
    
    return ei;
}

void PlanetsRenderingAspectImpl::on_render_event(RenderPassNodeGraph::RenderPassEvent p_evt, const dsstring& p_pass)
{
    if (RenderPassNodeGraph::RenderPassEvent::RENDERINGQUEUE_PASS_BEGIN == p_evt)
    {
        m_drawable.SetCurrentPass(p_pass);
    }
}

void PlanetsRenderingAspectImpl::create_camera_collisions(PlanetsRenderingAspectImpl::RegisteredCamera& p_cameradescr, bool p_hotstate)
{
    for (size_t i = 0; i < m_config.m_layers_descr.size(); i++)
    {
        LOD::Body* slod_body = _DRAWSPACE_NEW_(LOD::Body, LOD::Body(&m_config, i, &m_subpass_creation_cb, m_config.m_nbLODRanges_freeCameras, m_config.m_layers_descr[i].description));
        LOD::Layer* layer = _DRAWSPACE_NEW_(LOD::Layer, LOD::Layer(m_entitynodegraph, &m_config, slod_body, &m_subpass_creation_cb, &m_collisionmeshe_update_cb, i));

        layer->SetHotState(p_hotstate);

        p_cameradescr.layers.push_back(layer);
        m_layers_list.push_back(layer);
        
        if (m_enable_collisionmeshe_display)
        {
            // connect collision meshe creation update/event to Drawing
            if (0 == i) // for layer 0 (planet ground) only
            {
                layer->RegisterNewCollisionMesheCreationHandler(m_drawable.GetNewCollisionMesheCreationCb());
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

std::vector<PlanetsRenderingAspectImpl::RegisteredBody> PlanetsRenderingAspectImpl::get_bodies_list_from_registered_cams()
{
    std::vector<RegisteredBody> bodies;

    for (auto& cam : m_registered_camerapoints)
    {
        // search if has parent with rigidbodytransformaspectimpl, to apply gravity on it when it is relative (see manage_gravity_targets)
        std::vector<Entity*> ancestors;
        m_entitynodegraph->GetEntityAncestorsList(cam.second.owner_entity, ancestors);
        for (auto& e : ancestors)
        {
            TransformAspect* curr_transforms_aspect{ e->GetAspect<TransformAspect>() };
            if (curr_transforms_aspect)
            {
                auto impls{ curr_transforms_aspect->GetTransformAspectImplsList() };
                for (auto& impl : impls)
                {
                    RigidBodyTransformAspectImpl* rigidbodyimpl = dynamic_cast<RigidBodyTransformAspectImpl*>(impl.second);
                    if (rigidbodyimpl)
                    {
                        RegisteredBody body{ &m_registered_camerapoints.at(cam.second.camera_name), curr_transforms_aspect };
                        m_registered_camerapoints.at(cam.second.camera_name).parent_body_transform_aspect = curr_transforms_aspect;
                        bodies.push_back(body);
                    }
                }
            }
        }
    }
    return bodies;
}

void PlanetsRenderingAspectImpl::manage_gravity_targets(void)
{
    auto bodies{ get_bodies_list_from_registered_cams() };

    for (auto& body : bodies)
    {
        TransformAspect* curr_transforms_aspect{ body.transform_aspect };
        auto impls{ curr_transforms_aspect->GetTransformAspectImplsList() };

        for (auto& impl : impls)
        {
            RigidBodyTransformAspectImpl* rigidbodyimpl = dynamic_cast<RigidBodyTransformAspectImpl*>(impl.second);
            if (body.owner->layers[0]->GetHotState()) // if relative
            {
                auto m = rigidbodyimpl->GetLastLocalTransform();

                // gravity vector : from rigid body pos to planet center (which is 0,0,0 in planet physic world)
                Vector grav_to_planet(-m(3, 0), -m(3, 1), -m(3, 2), 1.0);

                grav_to_planet.Normalize();
                grav_to_planet.Scale(m_gravity_acc);

                dsstring gravity_force_comp_name{ dsstring("planet_gravity_") + std::to_string( (int)this )};

                Component<RigidBodyTransformAspectImpl::Force>* force = body.transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Force>(gravity_force_comp_name);
                if (nullptr == force)
                {
                    // component does not exists, create it
                    body.transform_aspect->AddComponent<RigidBodyTransformAspectImpl::Force>(gravity_force_comp_name, grav_to_planet, RigidBodyTransformAspectImpl::Force::GLOBAL, true);
                }
                else
                {
                    body.transform_aspect->GetComponent<RigidBodyTransformAspectImpl::Force>(gravity_force_comp_name)->getPurpose().UpdateForce(grav_to_planet);
                }
                break;
            }
        }
    }
}


void PlanetsRenderingAspectImpl::manage_camerapoints(void)
{
    Matrix planet_world;
    TransformAspect* transform_aspect = m_owner_entity->GetAspect<TransformAspect>();
    if (transform_aspect)
    {        
        transform_aspect->GetWorldTransform(planet_world);
    }
    else
    {
        _DSEXCEPTION("Planet must have transform aspect!!!")
    }

    Matrix planet_world_inv = planet_world;

    // clear translations
    planet_world_inv(3, 0) = 0.0;
    planet_world_inv(3, 1) = 0.0;
    planet_world_inv(3, 2) = 0.0;

    // matrix inversion
    planet_world_inv.Inverse();

    for(auto& camera: m_registered_camerapoints)
    {
        // process all type of cameras 

        TransformAspect* camera_transform_aspect{ camera.second.owner_entity->GetAspect<TransformAspect>() };
        if (camera_transform_aspect)
        {
            Matrix camera_world;
            camera_transform_aspect->GetWorldTransform(camera_world);

            DrawSpace::Utils::Vector camera_pos_from_planet;
            camera_pos_from_planet[0] = camera_world(3, 0) - planet_world(3, 0);
            camera_pos_from_planet[1] = camera_world(3, 1) - planet_world(3, 1);
            camera_pos_from_planet[2] = camera_world(3, 2) - planet_world(3, 2);
            camera_pos_from_planet[3] = 1.0;


            DrawSpace::Utils::Vector locale_camera_pos_from_planet;

            planet_world_inv.Transform(&camera_pos_from_planet, &locale_camera_pos_from_planet);

            dsreal rel_alt = (locale_camera_pos_from_planet.Length() / m_planet_ray);

            camera.second.relative_alt_valid = true;
            camera.second.relative_alt = rel_alt;

            camera.second.locale_camera_pos_from_planet = locale_camera_pos_from_planet;
            Vector spherical;
            Maths::CartesiantoSpherical(locale_camera_pos_from_planet, spherical);

            camera.second.locale_camera_long_lat[0] = spherical[0];
            camera.second.locale_camera_long_lat[1] = Utils::Maths::RadToDeg( spherical[1] );
            camera.second.locale_camera_long_lat[2] = Utils::Maths::RadToDeg( spherical[2] );

            camera.second.global_camera_pos_from_planet = camera_pos_from_planet;


            for(auto& camera_layer: camera.second.layers)
            {
                camera_layer->UpdateRelativeAlt( rel_alt );
                camera_layer->UpdateInvariantViewerPos( camera_pos_from_planet );

                if (camera_layer->GetHotState())
                {
                    camera_layer->UpdateHotPoint(locale_camera_pos_from_planet);
                    camera_layer->Compute();

                    if (rel_alt >= LOD::cst::hotRelativeAlt * 1.2)
                    {
                        camera_layer->SetHotState(false);
                        camera_layer->ResetBody();
                        camera_layer->RemoveCollider();
                        m_drawable.ResetCollisionMesheValidity();
                    }
                }
                else
                {
                    if (rel_alt < LOD::cst::hotRelativeAlt)
                    {
                        camera_layer->SetHotState(true);
                    }
                }
            }
        }
        else
        {
            _DSEXCEPTION("Camera must have transform aspect!!!")
        }
    }
}


// needed renderstate behaviours for surface layer (0)
//      main_pass
//      --> under threshold : ENABLE ZBUFFER
//      --> over threshold : DISABLE ZBUFFER
//      reflection pass
//      --> N/A

// needed renderstate behaviours for flatclouds layer (2)
//      main_pass
//      --> under threshold : ENABLE ZBUFFER, CULLING CCW
//      --> over threshold : DISABLE ZBUFFER, CULLING CW
//      reflection_pass
//      --> under threshold : ENABLE ZBUFFER, CULLING CW 
//      --> over threshold : ENABLE ZBUFFER, CULLING CW --> CW is default behaviour : no need to specify anything
//  

// needed renderstate behaviours for atmo layer (1)
//      main_pass
//      --> DISABLE ZBUFFER, CULLING CCW   --> setting in lua model file is enough

//      reflection_pass
//      --> DISABLE ZBUFFER, CULLING CW --> CW is default behaviour : no need to specify anything


void PlanetsRenderingAspectImpl::details_control_from_viewer_alt(void)
{
    if (m_current_camera && m_current_camera->relative_alt_valid)
    {        
        dsreal view_rel_alt{ m_current_camera->relative_alt };        
        dsreal zbuffer_activation_rel_alt{ m_owner->GetComponent<dsreal>("zbufferactivationrelalt")->getPurpose() };

        if (view_rel_alt < zbuffer_activation_rel_alt)
        {           
            auto rs_pair{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "true"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false")) };          
            std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>      renderstate_per_passes =
            {
                { m_main_pass, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>(1, rs_pair) }
            };

            m_drawable.SetRenderStatePerPassTableForLayer(DetailsLayer, renderstate_per_passes);
        }
        else
        {
            auto rs_pair{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false")) };
            std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>      renderstate_per_passes =
            {
                { m_main_pass, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>(1, rs_pair) }
            };

            m_drawable.SetRenderStatePerPassTableForLayer(DetailsLayer, renderstate_per_passes);
        }
    }
}

        


void PlanetsRenderingAspectImpl::flatclouds_control_from_viewer_alt(void)
{
    if (m_current_camera && m_current_camera->relative_alt_valid)
    {
        bool relative{ m_current_camera->layers[0]->GetHotState() };
        dsreal altitude{ m_current_camera->layers[0]->GetBody()->GetHotPointAltitud() };

        dsreal flatclouds_altitude{ m_owner->GetComponent<dsreal>("flatclouds_altitude")->getPurpose() };

        if (relative && altitude < flatclouds_altitude * 1000.0)
        {
            std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>> rs_list_mainpass;
            {
                auto rs_pair_zbuff{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "true"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false")) };
                auto rs_pair_culling{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "ccw"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "cw")) };

                rs_list_mainpass.push_back(rs_pair_zbuff);
                rs_list_mainpass.push_back(rs_pair_culling);
            }

            std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>> rs_list_reflectionpass;
            {
                auto rs_pair_zbuff{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "true"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false")) };

                // not needed, cw is default behaviour
                //auto rs_pair_culling{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "cw"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "cw")) };

                rs_list_reflectionpass.push_back(rs_pair_zbuff);
                //rs_list_reflectionpass.push_back(rs_pair_culling);
            }

            std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>      renderstate_per_passes =
            {
                { m_main_pass, rs_list_mainpass },
                { m_reflection_pass, rs_list_reflectionpass }
            };

            m_drawable.SetRenderStatePerPassTableForLayer(FlatCloudsLayer, renderstate_per_passes);
        }
        else
        {
            std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>> rs_list_mainpass;
            {
                auto rs_pair_zbuff{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false")) };

                // not needed, cw is default behaviour
                //auto rs_pair_culling{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "cw"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "cw")) };

                rs_list_mainpass.push_back(rs_pair_zbuff);
                //rs_list_mainpass.push_back(rs_pair_culling);
            }

            std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>> rs_list_reflectionpass;
            {
                auto rs_pair_zbuff{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "true"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false")) };
                auto rs_pair_culling{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "cw"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "cw")) };

                rs_list_reflectionpass.push_back(rs_pair_zbuff);
                rs_list_reflectionpass.push_back(rs_pair_culling);
            }

            std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>      renderstate_per_passes =
            {
                { m_main_pass, rs_list_mainpass },
                { m_reflection_pass, rs_list_reflectionpass }
            };

            m_drawable.SetRenderStatePerPassTableForLayer(FlatCloudsLayer, renderstate_per_passes);
        }
    }
}

void PlanetsRenderingAspectImpl::oceans_control_from_viewer_alt(void)
{
    if (m_current_camera && m_current_camera->relative_alt_valid)
    {
        bool relative{ m_current_camera->layers[0]->GetHotState() };

        dsreal relative_altitude{ m_current_camera->relative_alt };

        dsreal ocean_details_limit{ m_owner->GetComponent<dsreal>("splat_transition_down_relative_alt")->getPurpose() };

        if (relative && relative_altitude < ocean_details_limit)
        {
            m_drawable.SetLayerNodeDrawingState(OceansLayer, true);
        }
        else
        {
            m_drawable.SetLayerNodeDrawingState(OceansLayer, false);
        }

        if (relative && relative_altitude < 1.0)
        {

            ////////////// set ccw culling
            std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>> rs_list_mainpass;
            {
                auto rs_pair_culling{ std::make_pair(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "ccw"), DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETCULLING, "cw")) };

                rs_list_mainpass.push_back(rs_pair_culling);
            }

            std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>      renderstate_per_passes =
            {
                { m_oceanmask_pass, rs_list_mainpass },
                { m_bump_pass, rs_list_mainpass }
            };
            m_drawable.SetRenderStatePerPassTableForLayer(OceansLayer, renderstate_per_passes);

            ////////////////////// and set DrawPatchMode for the oceanmask rendering node
            auto nodes{ m_drawable.GetFaceDrawingNode() };
            for (auto& node : nodes)
            {
                if (node.first == m_oceanmask_pass)
                {
                    if (OceansLayer == node.second->GetLayerIndex())
                    {
                        node.second->SetDrawPatchMode(LOD::FaceDrawingNode::DRAW_MAXLODLEVEL, 3);
                    }
                }
            }
        }
        else
        {
            std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>> rs_list_mainpass;
            {
                // sending void list here, cw is default behaviour
            }

            std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>      renderstate_per_passes =
            {
                { m_oceanmask_pass, rs_list_mainpass },
                { m_bump_pass, rs_list_mainpass }
            };

            m_drawable.SetRenderStatePerPassTableForLayer(OceansLayer, renderstate_per_passes);


            ////////////////////// set DrawPatchMode for the oceanmask rendering node
            auto nodes{ m_drawable.GetFaceDrawingNode() };
            for (auto& node : nodes)
            {
                if (node.first == m_oceanmask_pass)
                {
                    if (OceansLayer == node.second->GetLayerIndex())
                    {
                        node.second->SetDrawPatchMode(LOD::FaceDrawingNode::DRAW_ALL);
                    }
                }
            }

        }
    }
    else
    {
        m_drawable.SetLayerNodeDrawingState(OceansLayer, false);
    }
}

void PlanetsRenderingAspectImpl::setup_collisions_aspect(void)
{
    m_owner_entity->AddAspect<CollisionAspect>();

    // for collisions management
    CollisionAspect* collision_aspect{ m_owner_entity->GetAspect<CollisionAspect>() };
    if (NULL == collision_aspect)
    {
        _DSEXCEPTION("Collision aspect doesnt exists in Planet entity!");
    }

    // add bool component for contact state
    collision_aspect->AddComponent<bool>("contact_state", false);

    collision_aspect->AddComponent<CollisionAspect::CompoundCollisionShape>("global_shape");
}

void PlanetsRenderingAspectImpl::release_collisions_aspect(void)
{   
    CollisionAspect* collision_aspect{ m_owner_entity->GetAspect<CollisionAspect>() };

    collision_aspect->RemoveComponent<bool>("contact_state");
    collision_aspect->RemoveComponent<CollisionAspect::CompoundCollisionShape>("global_shape");

    m_owner_entity->RemoveAspect<CollisionAspect>();
}

void PlanetsRenderingAspectImpl::ResetCollisionMesheValidity(void)
{
    m_drawable.ResetCollisionMesheValidity();
}

std::map<dsstring, PlanetsRenderingAspectImpl::RegisteredCamera> PlanetsRenderingAspectImpl::GetRegisteredCameras(void) const
{
    return m_registered_camerapoints;
}

std::map<dsstring, std::array<PlanetDetailsBinder*, 6>> PlanetsRenderingAspectImpl::GetPlanetFlatCloudsBinder(void) const
{
    return m_planet_flatclouds_binder;
}

std::map<dsstring, std::array<PlanetDetailsBinder*, 6>> PlanetsRenderingAspectImpl::GetPlanetAtmoBinder(void) const
{
    return m_planet_atmosphere_binder;
}

dsstring PlanetsRenderingAspectImpl::GetReflectionPassId(void) const
{
    return m_reflection_pass;
}
