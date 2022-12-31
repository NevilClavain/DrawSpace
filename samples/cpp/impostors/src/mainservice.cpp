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

#include "mainservice.h"
#include "component.h"
#include "passslot.h"

#include "logconf.h"
#include "callback.h"

#include "serviceaspect.h"
#include "physicsaspect.h"
#include "transformaspect.h"
#include "collisionaspect.h"
#include "cameraaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

_DECLARE_DS_LOGGER( logger, "test_impostors_mainservice", NULL )


extern DrawSpace::Logger::Sink aspect_logger;
extern DrawSpace::Logger::Sink rs_logger;       //resource system logger
extern DrawSpace::Logger::Sink rd_logger;       //renderingqueue system logger

extern DrawSpace::Logger::Sink runner_logger;

MainService::MainService( void ) :
m_resource_events_cb(this, &MainService::on_resource_event)
{
    DrawSpace::Systems::ResourcesSystem::SetTexturesRootPath("impostorsdemo_assets/textures_bank");
    DrawSpace::Systems::ResourcesSystem::SetShadersRootPath("impostorsdemo_assets/shaders_bank");
    DrawSpace::Systems::ResourcesSystem::EnableShadersDescrInFinalPath(true);

    DrawSpace::Systems::ResourcesSystem::SetMeshesRootPath("impostorsdemo_assets/meshes_bank");

    //File::MountVirtualFS("test_data.bank");
}

bool MainService::Init( void )
{
    //////////////recup params du service //////////////////

    ComponentList<DrawSpace::Logger::Configuration*> logconfs;
    m_owner->GetComponentsByType<DrawSpace::Logger::Configuration*>( logconfs );

    ComponentList<DrawSpace::Core::BaseCallback<void, bool>*> mouse_cbs;
    m_owner->GetComponentsByType<DrawSpace::Core::BaseCallback<void, bool>*>( mouse_cbs );

    ComponentList<DrawSpace::Core::BaseCallback<void, int>*> app_cbs;
    m_owner->GetComponentsByType<DrawSpace::Core::BaseCallback<void, int>*>( app_cbs );

    DrawSpace::Core::BaseCallback<void, bool>* mousecircularmode_cb;
    DrawSpace::Core::BaseCallback<void, bool>* mousevisible_cb;

    mousecircularmode_cb = mouse_cbs[0]->getPurpose();
    mousevisible_cb = mouse_cbs[1]->getPurpose();

    DrawSpace::Logger::Configuration* logconf = logconfs[0]->getPurpose();

    DrawSpace::Core::BaseCallback<void, int>* closeapp_cb;
    closeapp_cb = app_cbs[0]->getPurpose();


    m_mousecircularmode_cb = mousecircularmode_cb;
    m_closeapp_cb = closeapp_cb;

    ////////////////////////////////////////////////////////

        
    logconf->RegisterSink( &logger );
    logger.SetConfiguration( logconf );

    logconf->RegisterSink(&aspect_logger);
    aspect_logger.SetConfiguration(logconf);

    logconf->RegisterSink(&rs_logger);
    rs_logger.SetConfiguration(logconf);

    logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( logconf );

    logconf->RegisterSink(&rd_logger);
    rd_logger.SetConfiguration(logconf);

    logconf->RegisterSink(&runner_logger);
    runner_logger.SetConfiguration(logconf);

    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    /////////////////////////////////////////////////////////////////////////////////

    m_rendergraph.SetSystemsHub(&m_systemsHub);

    m_systemsHub.StartupRunner();

    Systems::ResourcesSystem& resources_system{ m_systemsHub.GetSystem<Systems::ResourcesSystem>("ResourcesSystem") };
    resources_system.RegisterEventHandler(&m_resource_events_cb);


    /////////////////////////////////////////////////////////////////////////////////

    m_finalpass = m_rendergraph.CreateRoot( "final_pass" );

    m_finalpass.CreateViewportQuad();

    m_finalpass.GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    RenderStatesSet finalpass_rss;
    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "point" ) );
    finalpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );


    m_finalpass.GetViewportQuad()->GetFx()->SetRenderStates( finalpass_rss );


    m_finalpass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_vs.hlsl", false ) ) );
    m_finalpass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_ps.hlsl", false ) ) );




	resources_system.LoadShader(m_finalpass.GetViewportQuad()->GetFx()->GetShader(0), 0);
	resources_system.LoadShader(m_finalpass.GetViewportQuad()->GetFx()->GetShader(1), 1);



    m_finalpass.GetRenderingQueue()->EnableDepthClearing( false );
    //m_finalpass.GetRenderingQueue()->EnableTargetClearing( false );
    m_finalpass.GetRenderingQueue()->EnableTargetClearing(true);
    m_finalpass.GetRenderingQueue()->SetTargetClearingColor(25, 25, 25, 255);



    m_texturepass = m_finalpass.CreateChild( "texture_pass", 0 );    
    m_texturepass.GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass.GetRenderingQueue()->EnableTargetClearing( false );

    ////////////////////////////////////////////////////////////////////////////////////


    TimeAspect* time_aspect = m_rootEntity.AddAspect<TimeAspect>();

    time_aspect->AddComponent<TimeManager>("time_manager");
    time_aspect->AddComponent<TimeAspect::TimeScale>("time_scale", TimeAspect::TimeScale::NORMAL_TIME);
    time_aspect->AddComponent<dsstring>("output_formated_datetime", "...");
    time_aspect->AddComponent<dstime>("time", 0);
    time_aspect->AddComponent<int>("output_fps");
    time_aspect->AddComponent<int>("output_world_nbsteps");

    time_aspect->AddComponent<dsreal>("output_time_factor");

    m_fps_yaw = time_aspect->TimeAngleFactory(0.0);
    m_fps_pitch = time_aspect->TimeAngleFactory(0.0);



    RenderingAspect* rendering_aspect = m_rootEntity.AddAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_passesRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());
    m_passesRender.SetRendergraph( &m_rendergraph );


    rendering_aspect->AddImplementation( &m_textRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());
    rendering_aspect->AddComponent<StringRenderingAspectImpl::TextDisplay>( "fps", 10, 20, 255, 100, 100, "..." );


    rendering_aspect->AddImplementation(&m_entityIdTextRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());
    rendering_aspect->AddComponent<StringRenderingAspectImpl::TextDisplay>("entityId", 0, 0, 0, 255, 0, "Entity0");

    rendering_aspect->AddImplementation(&m_entityDistanceTextRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());
    rendering_aspect->AddComponent<StringRenderingAspectImpl::TextDisplay>("entityDistance", 0, 0, 0, 255, 0, "?");






    m_rootEntityNode = m_entitygraph.SetRoot( &m_rootEntity );

    //////////////////////////////////////////////////////////////////////////////////////

    PhysicsAspect* physic_aspect = m_world1Entity.AddAspect<PhysicsAspect>();

    physic_aspect->AddComponent<bool>( "gravity_state", true );    
    physic_aspect->AddComponent<Vector>( "gravity", Vector( 0.0, -9.81, 0.0, 0.0 ) );

    m_World1EntityNode = m_rootEntityNode.AddChild( &m_world1Entity );


    /////////////////////////////////////////////////////////////////////////////////////

    create_camera();
    create_skybox();

    
    create_ground();
    create_cube( 0.0, 9.0, 10.0, m_cubeRender, m_cubeEntity, m_rigbody_cube_transformer);
    create_cube(0.0, 15.0, 14.0, m_cube2Render, m_cube2Entity, m_rigbody_cube_transformer2);
    create_composition(-13.0, 28.0, -4.0, m_mainBodyRender, m_mainBodyEntity, m_composition_transformer, m_feetRender, m_feetEntity, m_rigbody_composition_transformer);
    create_screen_impostors();
    create_world_impostor();
    create_sprite_impostor();
    create_collimator_sprite_impostor();



    // ajouter la skybox a la scene
    m_skyboxEntityNode = m_rootEntityNode.AddChild( &m_skyboxEntity );

    m_skyboxRender.RegisterToRendering(m_rendergraph);

    // ajouter la cameras a la scene
    m_cameraEntityNode = m_rootEntityNode.AddChild( &m_cameraEntity );

    // designer la camera courante    
    m_entitygraph.SetCurrentCameraEntity( &m_cameraEntity );

    
    
    // ajouter le ground a la scene
    m_groundEntityNode = m_World1EntityNode.AddChild( &m_groundEntity );
    m_groundRender.RegisterToRendering( m_rendergraph );


    // ajouter le cube a la scene
    m_cubeEntityNode = m_World1EntityNode.AddChild(&m_cubeEntity);
    m_cubeRender.RegisterToRendering(m_rendergraph);

    // ajouter le cube a la scene
    m_cube2EntityNode = m_World1EntityNode.AddChild(&m_cube2Entity);
    m_cube2Render.RegisterToRendering(m_rendergraph);



    m_mainBodyEntityNode = m_World1EntityNode.AddChild(&m_mainBodyEntity);
    m_mainBodyRender.RegisterToRendering(m_rendergraph);


    m_feetEntityNode = m_mainBodyEntityNode.AddChild(&m_feetEntity);
    m_feetRender.RegisterToRendering(m_rendergraph);



    // ajout du champ d'impostors a la scene
    m_impostorsEntityNode = m_World1EntityNode.AddChild( &m_impostorsEntity );
    m_impostorsRender.RegisterToRendering( m_rendergraph );



    // ajout de l'impostor world a la scene
    m_worldImpostorsEntityNode = m_World1EntityNode.AddChild( &m_worldImpostorsEntity );
    m_worldImpostorsRender.RegisterToRendering( m_rendergraph );

    // ajout sprite a la scene
    m_spriteEntityNode = m_World1EntityNode.AddChild(&m_spriteEntity);
    m_spriteRender.RegisterToRendering(m_rendergraph);



    m_spriteCollimatorEntityNode = m_World1EntityNode.AddChild(&m_spriteCollimatorEntity);
    m_spriteCollimatorRender.RegisterToRendering(m_rendergraph);



    m_systemsHub.GetSystem<Systems::ResourcesSystem>("ResourcesSystem").Activate("INIT");

    /////////////////////////////////////////////////////////////////////////////////

    m_entitygraph.PushSignal_RenderSceneBegin();

    set_mouse_circular_mode( true );

    _DSDEBUG( logger, dsstring("MainService : startup...") );

    return true;
}


void MainService::Run( void )
{
    DrawSpace::Interface::Renderer* renderer{ DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface };
    DrawSpace::Interface::Renderer::Characteristics characteristics;
    renderer->GetRenderCharacteristics(characteristics);



    m_systemsHub.Run( &m_entitygraph );
    
    TimeAspect* time_aspect = m_rootEntity.GetAspect<TimeAspect>();
    char comment[256];

    sprintf( comment, "%d fps - %s", time_aspect->GetComponent<int>( "output_fps" )->getPurpose(), m_pluginDescr.c_str() );

    RenderingAspect* rendering_aspect = m_rootEntity.GetAspect<RenderingAspect>();
    rendering_aspect->GetComponent<StringRenderingAspectImpl::TextDisplay>( "fps" )->getPurpose().m_text = comment;


    /////////////////////////////
        
    TransformAspect* bodyTransform{ m_mainBodyEntity.GetAspect<TransformAspect>() };
    dsreal spx, spy;
    bool is_behind{ bodyTransform->ProjectLocalPoint(Vector(0.0, 0.0, 0.0, 1.0), spx, spy) };
    if (is_behind)
    {
        // hide by setting 2D pos out of the viewport
        spx = spx = 100.0;
    }

    m_spritecollimator_texturepass_rnode->SetShaderRealVector("pos2D", Vector(spx, spy, 0.0, 0.0));


    //////

    dsreal text_pos_x = spx - 0.05;
    dsreal text_pos_y = spy + 0.1;
         

    int tpos_x = (dsreal)characteristics.width_resol * 0.5 * (text_pos_x + 1.0);
    int tpos_y = (dsreal)characteristics.height_resol * 0.5 * (text_pos_y + 1.0);

    rendering_aspect->GetComponent<StringRenderingAspectImpl::TextDisplay>("entityId")->getPurpose().m_posx = tpos_x;
    rendering_aspect->GetComponent<StringRenderingAspectImpl::TextDisplay>("entityId")->getPurpose().m_posy = characteristics.height_resol - tpos_y;

    //////


    m_entityDistance = bodyTransform->LocalPointDistanceFromCamera(Vector(0.0, 0.0, 0.0, 1.0));



    dsreal text2_pos_x = spx - 0.05;
    dsreal text2_pos_y = spy - 0.1;


    int t2pos_x = (dsreal)characteristics.width_resol * 0.5 * (text2_pos_x + 1.0);
    int t2pos_y = (dsreal)characteristics.height_resol * 0.5 * (text2_pos_y + 1.0);


    rendering_aspect->GetComponent<StringRenderingAspectImpl::TextDisplay>("entityDistance")->getPurpose().m_posx = t2pos_x;
    rendering_aspect->GetComponent<StringRenderingAspectImpl::TextDisplay>("entityDistance")->getPurpose().m_posy = characteristics.height_resol - t2pos_y;

    rendering_aspect->GetComponent<StringRenderingAspectImpl::TextDisplay>("entityDistance")->getPurpose().m_text = std::to_string(m_entityDistance);



    /////////////////////////////
}

void MainService::Release( void )
{
    _DSDEBUG( logger, dsstring("MainService : shutdown...") );

    m_systemsHub.ShutdownRunner();

    m_systemsHub.ReleaseAssets();
}

void MainService::create_skybox(void)
{
    RenderingAspect* rendering_aspect{ m_skyboxEntity.AddAspect<RenderingAspect>() };
    TimeAspect* time_aspect{ m_rootEntity.GetAspect<TimeAspect>() };

    rendering_aspect->AddImplementation(&m_skyboxRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());

    std::map<dsstring, int>                                 rcname_to_layer_index   { { "main_rendering", 0 } };
    std::map<dsstring, std::vector<dsstring>>               rcname_to_passes        { { "main_rendering", { { "texture_pass" } } } };

    ////////////// 6 jeux de 32 textures stages

    ///////////////// jeux de textures pour chaque passes


    std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>> skybox_textures;

    std::array<std::array<Texture*, RenderingNode::NbMaxTextures>, 6> textures = { NULL };
    textures[0][0] = _DRAWSPACE_NEW_(Texture, Texture("sb0.bmp"));
    textures[1][0] = _DRAWSPACE_NEW_(Texture, Texture("sb2.bmp"));
    textures[2][0] = _DRAWSPACE_NEW_(Texture, Texture("sb3.bmp"));
    textures[3][0] = _DRAWSPACE_NEW_(Texture, Texture("sb1.bmp"));
    textures[4][0] = _DRAWSPACE_NEW_(Texture, Texture("sb4.bmp"));
    textures[5][0] = _DRAWSPACE_NEW_(Texture, Texture("sb4.bmp"));

    for (int i = 0; i < 6; i++)
    {
        skybox_textures.push_back(textures[i]);
    }

    std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>   layers_textures = { { { "main_rendering", skybox_textures } } };


    /////////////// les FX pour chaque passes

    Fx* skybox_texturepass_fx = _DRAWSPACE_NEW_(Fx, Fx);

    skybox_texturepass_fx->AddShader(_DRAWSPACE_NEW_(Shader, Shader("texture_vs.hlsl", false)));
    skybox_texturepass_fx->AddShader(_DRAWSPACE_NEW_(Shader, Shader("texture_ps.hlsl", false)));

    RenderStatesSet skybox_texturepass_rss;
    skybox_texturepass_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));
    skybox_texturepass_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));

    skybox_texturepass_fx->SetRenderStates(skybox_texturepass_rss);

    std::vector<std::map<dsstring, Fx*>>                                                               layers_fx = { { { "main_rendering", skybox_texturepass_fx } }  };


    /////////// params shaders

    std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>    layers_shaders_params = { { { "main_rendering", {} }} };

    //////////////// valeur du rendering order pour chaque slot pass

    //std::vector<std::vector<int>>                                                               layers_ro = { {-1000} };

    std::vector<std::map<dsstring, int>>                                                               layers_ro = { { { "main_rendering", -1000 } } };


    rendering_aspect->AddComponent<std::map<dsstring, std::vector<dsstring>>>("rcname_to_passes", rcname_to_passes);

    rendering_aspect->AddComponent<std::map<dsstring, int>>("rcname_to_layer_index", rcname_to_layer_index);


    rendering_aspect->AddComponent<std::vector<std::map<dsstring, std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>>>("layers_textures", layers_textures);
    rendering_aspect->AddComponent<std::vector<std::map<dsstring, Fx*>>>("layers_fx", layers_fx);
    rendering_aspect->AddComponent<std::vector<std::map<dsstring, std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>>("layers_shaders_params", layers_shaders_params);
    rendering_aspect->AddComponent<std::vector<std::map<dsstring, int>>>("layers_ro", layers_ro);




    /////////// resources ////////////////////////////////

    ResourcesAspect* resources_aspect = m_skyboxEntity.AddAspect<ResourcesAspect>();

    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("skybox_texture_0", std::make_tuple(textures[0][0], false));
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("skybox_texture_1", std::make_tuple(textures[1][0], false));
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("skybox_texture_2", std::make_tuple(textures[2][0], false));
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("skybox_texture_3", std::make_tuple(textures[3][0], false));
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("skybox_texture_4", std::make_tuple(textures[4][0], false));
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("skybox_texture_5", std::make_tuple(textures[5][0], false));

    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("skybox_vshader", std::make_tuple(skybox_texturepass_fx->GetShader(0), false, 0));
    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("skybox_pshader", std::make_tuple(skybox_texturepass_fx->GetShader(1), false, 1));




    TransformAspect* transform_aspect = m_skyboxEntity.AddAspect<TransformAspect>();

    transform_aspect->AddImplementation( 0, &m_skybox_transformer );

    transform_aspect->AddComponent<Matrix>( "skybox_scaling" );

    transform_aspect->GetComponent<Matrix>( "skybox_scaling" )->getPurpose().Scale( 100.0, 100.0, 100.0 );
        
}

void MainService::create_world_impostor( void )
{
    RenderingAspect* rendering_aspect = m_worldImpostorsEntity.AddAspect<RenderingAspect>();

    TimeAspect* time_aspect = m_rootEntity.GetAspect<TimeAspect>();

    rendering_aspect->AddImplementation( &m_worldImpostorsRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose() );

    rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::PassSlot>( "texturepass_slot", "texture_pass" );

    ImpostorsRenderingAspectImpl::ImpostorDescriptor id;

    
    id.localpos = Vector( 0.0, 1.0, 0.0, 1.0 );
    id.width_scale = 1.0;
    id.height_scale = 1.0;
    id.u1 = 0.0;
    id.v1 = 0.0;

    id.u2 = 1.0;
    id.v2 = 0.0;

    id.u3 = 1.0;
    id.v3 = 1.0;

    id.u4 = 0.0;
    id.v4 = 1.0;

    rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::ImpostorDescriptor>( "0", id );
    
    RenderingNode* impostors_texturepass_rnode = rendering_aspect->GetComponent<ImpostorsRenderingAspectImpl::PassSlot>( "texturepass_slot" )->getPurpose().GetRenderingNode();


    impostors_texturepass_rnode->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );


    impostors_texturepass_rnode->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor_vs.hlsl", false ) ) );
    impostors_texturepass_rnode->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor_ps.hlsl", false ) ) );


	Systems::ResourcesSystem& resources_system = m_systemsHub.GetSystem<Systems::ResourcesSystem>("ResourcesSystem");
	resources_system.LoadShader(impostors_texturepass_rnode->GetFx()->GetShader(0), 0);
	resources_system.LoadShader(impostors_texturepass_rnode->GetFx()->GetShader(1), 1);


    impostors_texturepass_rnode->AddShaderParameter(0, "vflags", 24);
    impostors_texturepass_rnode->SetShaderRealVector("vflags", Vector(0.0, 0.0, 0.0, 0.0));

    impostors_texturepass_rnode->AddShaderParameter( 1, "flags", 0 );
    impostors_texturepass_rnode->SetShaderRealVector( "flags", Vector( 0.0, 0.0, 0.0, 0.0 ) );

    impostors_texturepass_rnode->AddShaderParameter( 1, "color", 1 );
    impostors_texturepass_rnode->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );

    impostors_texturepass_rnode->SetOrderNumber(1000);

    RenderStatesSet impostors_texturepass_rss;
    impostors_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    impostors_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    impostors_texturepass_rnode->GetFx()->SetRenderStates( impostors_texturepass_rss );

    impostors_texturepass_rnode->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "map.jpg" ) ), 0 );

    /////////// resources ////////////////////////////////

    ResourcesAspect* resources_aspect = m_worldImpostorsEntity.AddAspect<ResourcesAspect>();
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("texture", std::make_tuple(impostors_texturepass_rnode->GetTexture(0), false));


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TransformAspect* transform_aspect = m_worldImpostorsEntity.AddAspect<TransformAspect>();

    transform_aspect->AddImplementation( 0, &m_impostors_transformer );

    transform_aspect->AddComponent<Matrix>( "pos" );

    transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( -15.0, 2.0, -12.0, 1.0) );

}


void MainService::create_screen_impostors( void )
{
    RenderingAspect* rendering_aspect = m_impostorsEntity.AddAspect<RenderingAspect>();

    TimeAspect* time_aspect = m_rootEntity.GetAspect<TimeAspect>();

    rendering_aspect->AddImplementation( &m_impostorsRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());

    rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::PassSlot>( "texturepass_slot", "texture_pass" );

    ImpostorsRenderingAspectImpl::ImpostorDescriptor id;
    id.width_scale = 0.1;
    id.height_scale = 0.1;
    id.u1 = 0.0;
    id.v1 = 0.0;

    id.u2 = 1.0;
    id.v2 = 0.0;

    id.u3 = 1.0;
    id.v3 = 1.0;

    id.u4 = 0.0;
    id.v4 = 1.0;


    int nb_w = 30;
    int nb_h = 30;

    dsreal field_w = 8.0;
    dsreal field_h = 8.0;

    dsreal w_step = field_w / ( nb_w - 1 );
    dsreal h_step = field_h / ( nb_h - 1 );
   
    dsreal w_pos;
    dsreal h_pos = -field_h * 0.5;

    dsstring particle_id;

    for( int h = 0; h < nb_h; h++ )
    {
        w_pos = -field_w * 0.5;

        for( int w = 0; w < nb_w; w++ )
        {
            id.localpos[0] = w_pos;
            id.localpos[1] = h_pos;

            id.localpos[2] = 0.0;

            char comment[64];
            sprintf( comment, "%f.%f", w_pos, h_pos );
            particle_id = comment;

            rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::ImpostorDescriptor>( particle_id, id );

            w_pos += w_step;
        }

        h_pos += h_step;
    }

    RenderingNode* impostors_texturepass_rnode = rendering_aspect->GetComponent<ImpostorsRenderingAspectImpl::PassSlot>( "texturepass_slot" )->getPurpose().GetRenderingNode();

    impostors_texturepass_rnode->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    impostors_texturepass_rnode->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor_vs.hlsl", false ) ) );
    impostors_texturepass_rnode->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor_ps.hlsl", false ) ) );

	Systems::ResourcesSystem& resources_system = m_systemsHub.GetSystem<Systems::ResourcesSystem>("ResourcesSystem");
	resources_system.LoadShader(impostors_texturepass_rnode->GetFx()->GetShader(0), 0);
	resources_system.LoadShader(impostors_texturepass_rnode->GetFx()->GetShader(1), 1);


    impostors_texturepass_rnode->AddShaderParameter( 0, "globalscale", 24 );
    impostors_texturepass_rnode->SetShaderRealVector( "globalscale", Vector( 5.0, 5.0, 0.0, 1.0 ) );

    impostors_texturepass_rnode->AddShaderParameter( 1, "flags", 0 );
    impostors_texturepass_rnode->SetShaderRealVector( "flags", Vector( 0.0, 0.0, 0.0, 0.0 ) );

    impostors_texturepass_rnode->AddShaderParameter( 1, "color", 1 );
    impostors_texturepass_rnode->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );

    impostors_texturepass_rnode->SetOrderNumber( 1000 );

    RenderStatesSet impostors_texturepass_rss;

    impostors_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    impostors_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    impostors_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    impostors_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    impostors_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one" ) );
    impostors_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );
    


    impostors_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    impostors_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    impostors_texturepass_rnode->GetFx()->SetRenderStates( impostors_texturepass_rss );

    impostors_texturepass_rnode->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "star.bmp" ) ), 0 );    
    /////////// resources ////////////////////////////////

    ResourcesAspect* resources_aspect = m_impostorsEntity.AddAspect<ResourcesAspect>();
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("texture", std::make_tuple(impostors_texturepass_rnode->GetTexture(0), false));
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TransformAspect* transform_aspect = m_impostorsEntity.AddAspect<TransformAspect>();

    transform_aspect->AddImplementation( 0, &m_impostors_transformer );

    transform_aspect->AddComponent<Matrix>( "pos" );

    transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( 0.0, 6.0, -12.0, 1.0) );
}

void MainService::create_sprite_impostor(void)
{
    RenderingAspect* rendering_aspect{ m_spriteEntity.AddAspect<RenderingAspect>() };
    TimeAspect* time_aspect{ m_rootEntity.GetAspect<TimeAspect>() };

    rendering_aspect->AddImplementation(&m_spriteRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());
    rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::PassSlot>("texturepass_slot", "texture_pass");

    ImpostorsRenderingAspectImpl::ImpostorDescriptor id;
    id.localpos = Vector(0.0, 0.0, 0.0, 1.0);
    id.width_scale = 0.2;
    id.height_scale = 0.2;
    id.u1 = 0.0;
    id.v1 = 0.0;

    id.u2 = 1.0;
    id.v2 = 0.0;

    id.u3 = 1.0;
    id.v3 = 1.0;

    id.u4 = 0.0;
    id.v4 = 1.0;

    rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::ImpostorDescriptor>("0", id);
    RenderingNode* impostors_texturepass_rnode{ rendering_aspect->GetComponent<ImpostorsRenderingAspectImpl::PassSlot>("texturepass_slot")->getPurpose().GetRenderingNode() };

    impostors_texturepass_rnode->SetFx(_DRAWSPACE_NEW_(Fx, Fx));
    impostors_texturepass_rnode->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("spriteimpostor_vs.hlsl", false)));
    impostors_texturepass_rnode->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("spriteimpostor_ps.hlsl", false)));

    Systems::ResourcesSystem& resources_system{ m_systemsHub.GetSystem<Systems::ResourcesSystem>("ResourcesSystem") };
    resources_system.LoadShader(impostors_texturepass_rnode->GetFx()->GetShader(0), 0);
    resources_system.LoadShader(impostors_texturepass_rnode->GetFx()->GetShader(1), 1);

    impostors_texturepass_rnode->AddShaderParameter(0, "pos2D", 5);
    impostors_texturepass_rnode->SetShaderRealVector("pos2D", Vector(-0.7, -0.7, 0.0, 0.0));

    impostors_texturepass_rnode->AddShaderParameter(1, "flags", 0);
    impostors_texturepass_rnode->SetShaderRealVector("flags", Vector(0.0, 0.0, 0.0, 0.0));

    impostors_texturepass_rnode->AddShaderParameter(1, "color", 1);
    impostors_texturepass_rnode->SetShaderRealVector("color", Vector(1.0, 1.0, 1.0, 1.0));

    impostors_texturepass_rnode->SetOrderNumber(2000);

    m_sprite_texturepass_rnode = impostors_texturepass_rnode;

    RenderStatesSet impostors_texturepass_rss;

    impostors_texturepass_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));
    impostors_texturepass_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));

    impostors_texturepass_rnode->GetFx()->SetRenderStates(impostors_texturepass_rss);

    impostors_texturepass_rnode->SetTexture(_DRAWSPACE_NEW_(Texture, Texture("wolverine.jpg")), 0);

    ResourcesAspect* resources_aspect{ m_spriteEntity.AddAspect<ResourcesAspect>() };
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("texture", std::make_tuple(impostors_texturepass_rnode->GetTexture(0), false));

}

void MainService::create_collimator_sprite_impostor(void)
{
    RenderingAspect* rendering_aspect{ m_spriteCollimatorEntity.AddAspect<RenderingAspect>() };
    TimeAspect* time_aspect{ m_rootEntity.GetAspect<TimeAspect>() };

    rendering_aspect->AddImplementation(&m_spriteCollimatorRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());
    rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::PassSlot>("texturepass_slot", "texture_pass");

    DrawSpace::Interface::Renderer* renderer{ DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface };

    DrawSpace::Interface::Renderer::Characteristics characteristics;
    renderer->GetRenderCharacteristics(characteristics);

    dsreal aspect_ratio{ characteristics.width_viewport / characteristics.height_viewport };


    ImpostorsRenderingAspectImpl::ImpostorDescriptor id;
    id.localpos = Vector(0.0, 0.0, 0.0, 1.0);

    id.width_scale = 0.1;
    id.height_scale = id.width_scale * aspect_ratio;


    id.u1 = 0.0;
    id.v1 = 0.0;

    id.u2 = 1.0;
    id.v2 = 0.0;

    id.u3 = 1.0;
    id.v3 = 1.0;

    id.u4 = 0.0;
    id.v4 = 1.0;

    rendering_aspect->AddComponent<ImpostorsRenderingAspectImpl::ImpostorDescriptor>("0", id);
    RenderingNode* impostors_texturepass_rnode{ rendering_aspect->GetComponent<ImpostorsRenderingAspectImpl::PassSlot>("texturepass_slot")->getPurpose().GetRenderingNode() };

    impostors_texturepass_rnode->SetFx(_DRAWSPACE_NEW_(Fx, Fx));
    impostors_texturepass_rnode->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("spriteimpostor_vs.hlsl", false)));
    impostors_texturepass_rnode->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("spriteimpostor_ps.hlsl", false)));

    Systems::ResourcesSystem& resources_system{ m_systemsHub.GetSystem<Systems::ResourcesSystem>("ResourcesSystem") };
    resources_system.LoadShader(impostors_texturepass_rnode->GetFx()->GetShader(0), 0);
    resources_system.LoadShader(impostors_texturepass_rnode->GetFx()->GetShader(1), 1);

    impostors_texturepass_rnode->AddShaderParameter(0, "pos2D", 5);
    impostors_texturepass_rnode->SetShaderRealVector("pos2D", Vector(0.0, 0.0, 0.0, 0.0));

    impostors_texturepass_rnode->AddShaderParameter(1, "flags", 0);
    impostors_texturepass_rnode->SetShaderRealVector("flags", Vector(0.0, 1.0, 0.0, 0.0));

    impostors_texturepass_rnode->AddShaderParameter(1, "color", 1);
    impostors_texturepass_rnode->SetShaderRealVector("color", Vector(1.0, 1.0, 1.0, 1.0));

    impostors_texturepass_rnode->AddShaderParameter(1, "keycolor", 2);
    impostors_texturepass_rnode->SetShaderRealVector("keycolor", Vector(0.0, 0.0, 0.0, 1.0));

    impostors_texturepass_rnode->SetOrderNumber(2000);

    m_spritecollimator_texturepass_rnode = impostors_texturepass_rnode;

    RenderStatesSet impostors_texturepass_rss;

    impostors_texturepass_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));
    impostors_texturepass_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));

    impostors_texturepass_rnode->GetFx()->SetRenderStates(impostors_texturepass_rss);

    impostors_texturepass_rnode->SetTexture(_DRAWSPACE_NEW_(Texture, Texture("collimator.jpg")), 0);

    ResourcesAspect* resources_aspect{ m_spriteCollimatorEntity.AddAspect<ResourcesAspect>() };
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("texture", std::make_tuple(impostors_texturepass_rnode->GetTexture(0), false));
}

void MainService::create_cube( dsreal p_x, dsreal p_y, dsreal p_z, MeshRenderingAspectImpl& p_rendering_aspect_impl, DrawSpace::Core::Entity& p_entity,
                                DrawSpace::AspectImplementations::RigidBodyTransformAspectImpl& p_rigidBodyTransformAspectImpl)
{
    RenderingAspect* rendering_aspect = p_entity.AddAspect<RenderingAspect>();
    TimeAspect* time_aspect = m_rootEntity.GetAspect<TimeAspect>();

    rendering_aspect->AddImplementation(&p_rendering_aspect_impl, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());

    rendering_aspect->AddComponent<PassSlot>("texturepass_slot", "texture_pass");

    RenderingNode* cube_texturepass = rendering_aspect->GetComponent<PassSlot>("texturepass_slot")->getPurpose().GetRenderingNode();
    cube_texturepass->SetFx(_DRAWSPACE_NEW_(Fx, Fx));

    cube_texturepass->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("texture_vs.hlsl", false)));
    cube_texturepass->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("texture_ps.hlsl", false)));

    RenderStatesSet cube_texturepass_rss;
    cube_texturepass_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "true"));
    cube_texturepass_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));

    cube_texturepass->GetFx()->SetRenderStates(cube_texturepass_rss);

    cube_texturepass->SetTexture(_DRAWSPACE_NEW_(Texture, Texture("family.jpg")), 0);

    cube_texturepass->SetMeshe(_DRAWSPACE_NEW_(Meshe, Meshe));

    cube_texturepass->SetOrderNumber(1100);

    /////////// resources ////////////////////////////////

    ResourcesAspect* resources_aspect = p_entity.AddAspect<ResourcesAspect>();

    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("texture", std::make_tuple(cube_texturepass->GetTexture(0), false));

    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("vshader", std::make_tuple(cube_texturepass->GetFx()->GetShader(0), false, 0));
    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("pshader", std::make_tuple(cube_texturepass->GetFx()->GetShader(1), false, 1));

    resources_aspect->AddComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>("meshe", std::make_tuple(cube_texturepass->GetMeshe(), 
                                                                                                                "family_cube.ac", "cube_photos", false));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TransformAspect* transform_aspect{ p_entity.AddAspect<TransformAspect>() };
    transform_aspect->AddImplementation(0, &p_rigidBodyTransformAspectImpl);

    // add bool component for contact state
    transform_aspect->AddComponent<bool>("contact_state", false);

    // add bool component for linear speed stop
    transform_aspect->AddComponent<bool>("stop_linear_speed", false);

    // add bool component for angular speed stop
    transform_aspect->AddComponent<bool>("stop_angular_speed", false);

    transform_aspect->AddComponent<RigidBodyTransformAspectImpl::BoxCollisionShape>("shape", Vector(0.5, 0.5, 0.5, 1.0));

    Matrix cube_attitude;
    cube_attitude.Translation(p_x, p_y, p_z);
    transform_aspect->AddComponent<Matrix>("attitude", cube_attitude);
    transform_aspect->AddComponent<dsreal>("mass", 7.0);
}

void MainService::create_composition(dsreal p_x, dsreal p_y, dsreal p_z,
                                        MeshRenderingAspectImpl& p_rendering_aspect_impl,
                                        DrawSpace::Core::Entity& p_entity,
                                        DrawSpace::AspectImplementations::RawTransformAspectImpl& p_transform_impl,
                                        MeshRenderingAspectImpl& p_rendering_aspect_impl_2,
                                        DrawSpace::Core::Entity& p_entity_2,
                                        DrawSpace::AspectImplementations::RigidBodyTransformAspectImpl& p_rigidBodyTransformAspectImpl)
{
    {
        RenderingAspect* rendering_aspect = p_entity.AddAspect<RenderingAspect>();
        TimeAspect* time_aspect = m_rootEntity.GetAspect<TimeAspect>();

        rendering_aspect->AddImplementation(&p_rendering_aspect_impl, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());

        rendering_aspect->AddComponent<PassSlot>("texturepass_slot", "texture_pass");

        RenderingNode* cube_texturepass = rendering_aspect->GetComponent<PassSlot>("texturepass_slot")->getPurpose().GetRenderingNode();
        cube_texturepass->SetFx(_DRAWSPACE_NEW_(Fx, Fx));

        cube_texturepass->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("texture_vs.hlsl", false)));
        cube_texturepass->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("texture_ps.hlsl", false)));

        RenderStatesSet cube_texturepass_rss;
        cube_texturepass_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "true"));
        cube_texturepass_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));

        cube_texturepass->GetFx()->SetRenderStates(cube_texturepass_rss);

        cube_texturepass->SetTexture(_DRAWSPACE_NEW_(Texture, Texture("tex59.jpg")), 0);

        cube_texturepass->SetMeshe(_DRAWSPACE_NEW_(Meshe, Meshe));

        cube_texturepass->SetOrderNumber(1100);

        /////////// resources ////////////////////////////////

        ResourcesAspect* resources_aspect = p_entity.AddAspect<ResourcesAspect>();

        resources_aspect->AddComponent<std::tuple<Texture*, bool>>("texture", std::make_tuple(cube_texturepass->GetTexture(0), false));

        resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("vshader", std::make_tuple(cube_texturepass->GetFx()->GetShader(0), false, 0));
        resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("pshader", std::make_tuple(cube_texturepass->GetFx()->GetShader(1), false, 1));

        resources_aspect->AddComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>("meshe", std::make_tuple(cube_texturepass->GetMeshe(),
            "composition.ac", "main_body", false));


        //////// transform //////////////////////////////////

        TransformAspect* transform_aspect{ p_entity.AddAspect<TransformAspect>() };
        transform_aspect->AddImplementation(0, &p_rigidBodyTransformAspectImpl);

        // add bool component for contact state
        transform_aspect->AddComponent<bool>("contact_state", false);

        // add bool component for linear speed stop
        transform_aspect->AddComponent<bool>("stop_linear_speed", false);

        // add bool component for angular speed stop
        transform_aspect->AddComponent<bool>("stop_angular_speed", false);

        transform_aspect->AddComponent<RigidBodyTransformAspectImpl::BoxCollisionShape>("main_box", Vector(1.5, 1.0, 1.0, 1.0));

        Utils::Matrix feet_pos;
        feet_pos.Translation(1.22307, -3.31759, 0.01);

        transform_aspect->AddComponent<RigidBodyTransformAspectImpl::BoxCollisionShape>("feet", Vector(0.1, 2.5, 0.2, 1.0), feet_pos);

        transform_aspect->AddComponent<RigidBodyTransformAspectImpl::CompoundCollisionShape>("global_shape");

        

        Matrix cube_attitude;
        cube_attitude.Translation(p_x, p_y, p_z);
        transform_aspect->AddComponent<Matrix>("attitude", cube_attitude);
        transform_aspect->AddComponent<dsreal>("mass", 7.0);
    }
    /////////////////////////////////////////////////////////////////////////////////////////

    {
        RenderingAspect* rendering_aspect = p_entity_2.AddAspect<RenderingAspect>();
        TimeAspect* time_aspect = m_rootEntity.GetAspect<TimeAspect>();

        rendering_aspect->AddImplementation(&p_rendering_aspect_impl_2, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());

        rendering_aspect->AddComponent<PassSlot>("texturepass_slot", "texture_pass");

        RenderingNode* cube_texturepass = rendering_aspect->GetComponent<PassSlot>("texturepass_slot")->getPurpose().GetRenderingNode();
        cube_texturepass->SetFx(_DRAWSPACE_NEW_(Fx, Fx));

        cube_texturepass->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("texture_vs.hlsl", false)));
        cube_texturepass->GetFx()->AddShader(_DRAWSPACE_NEW_(Shader, Shader("texture_ps.hlsl", false)));

        RenderStatesSet cube_texturepass_rss;
        cube_texturepass_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "true"));
        cube_texturepass_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::ENABLEZBUFFER, "false"));

        cube_texturepass->GetFx()->SetRenderStates(cube_texturepass_rss);

        cube_texturepass->SetTexture(_DRAWSPACE_NEW_(Texture, Texture("tex59.jpg")), 0);

        cube_texturepass->SetMeshe(_DRAWSPACE_NEW_(Meshe, Meshe));

        cube_texturepass->SetOrderNumber(1100);

        /////////// resources ////////////////////////////////

        ResourcesAspect* resources_aspect = p_entity_2.AddAspect<ResourcesAspect>();

        resources_aspect->AddComponent<std::tuple<Texture*, bool>>("texture", std::make_tuple(cube_texturepass->GetTexture(0), false));

        resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("vshader", std::make_tuple(cube_texturepass->GetFx()->GetShader(0), false, 0));
        resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("pshader", std::make_tuple(cube_texturepass->GetFx()->GetShader(1), false, 1));

        resources_aspect->AddComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>("meshe", std::make_tuple(cube_texturepass->GetMeshe(),
            "composition.ac", "feet", false));


        //////// transform //////////////////////////////////

        
        TransformAspect* transform_aspect = p_entity_2.AddAspect<TransformAspect>();

        transform_aspect->AddImplementation(0, &p_transform_impl);

        transform_aspect->AddComponent<Matrix>("pos");
        transform_aspect->GetComponent<Matrix>("pos")->getPurpose().Identity();
        

    }
}


void MainService::create_ground( void )
{
    RenderingAspect* rendering_aspect = m_groundEntity.AddAspect<RenderingAspect>();

    TimeAspect* time_aspect = m_rootEntity.GetAspect<TimeAspect>();

    rendering_aspect->AddImplementation( &m_groundRender, &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose());

    rendering_aspect->AddComponent<PassSlot>( "texturepass_slot", "texture_pass" );

    
    RenderingNode* ground_texturepass = rendering_aspect->GetComponent<PassSlot>( "texturepass_slot" )->getPurpose().GetRenderingNode();
    ground_texturepass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    ground_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_vs.hlsl", false ) ) );
    ground_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_ps.hlsl", false) ) );

    RenderStatesSet ground_texturepass_rss;
    ground_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    ground_texturepass_rss.AddRenderStateIn(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "point"));

    ground_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    ground_texturepass_rss.AddRenderStateOut(DrawSpace::Core::RenderState(DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear"));

    ground_texturepass->GetFx()->SetRenderStates( ground_texturepass_rss );

    ground_texturepass->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "map.jpg" ) ), 0 );

    ground_texturepass->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    ground_texturepass->SetOrderNumber( -500 );

    /////////// resources ////////////////////////////////
    
    ResourcesAspect* resources_aspect = m_groundEntity.AddAspect<ResourcesAspect>();
    
    resources_aspect->AddComponent<std::tuple<Texture*, bool>>("ground_texture", std::make_tuple(ground_texturepass->GetTexture(0), false));

    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("ground_vshader", std::make_tuple(ground_texturepass->GetFx()->GetShader(0), false, 0));
    resources_aspect->AddComponent<std::tuple<Shader*, bool, int>>("ground_pshader", std::make_tuple(ground_texturepass->GetFx()->GetShader(1), false, 1));

    resources_aspect->AddComponent<std::tuple<Meshe*, dsstring, dsstring, bool>>("ground_meshe", std::make_tuple(ground_texturepass->GetMeshe(),
                                                                                                                "water.ac", "my_flat_mesh", false));
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TransformAspect* transform_aspect{ m_groundEntity.AddAspect<TransformAspect>() };
  
    transform_aspect->AddImplementation(0, &m_ground_transformer);
    transform_aspect->AddComponent<Matrix>("pos");
    transform_aspect->GetComponent<Matrix>("pos")->getPurpose().Translation(Vector(0.0, -4.0, 0.0, 1.0));


    CollisionAspect* collision_aspect{ m_groundEntity.AddAspect<CollisionAspect>() };

    collision_aspect->AddComponent<CollisionAspect::BoxCollisionShape>("shape", Vector(100.0, 0.0, 100.0, 1.0));

    // add bool component for contact state
    collision_aspect->AddComponent<bool>("contact_state", false);
}

void MainService::create_camera( void )
{
    TransformAspect* transform_aspect = m_cameraEntity.AddAspect<TransformAspect>();

    transform_aspect->AddImplementation( 0, &m_fps_transformer );
    transform_aspect->AddComponent<dsreal>( "yaw", 0.0 );
    transform_aspect->AddComponent<dsreal>( "pitch", 0.0 );

    transform_aspect->AddComponent<Vector>( "speed" );
    transform_aspect->AddComponent<Matrix>( "pos" );

    transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( 0.0, 2.0, 30.0, 1.0 ) );

    transform_aspect->AddComponent<bool>( "ymvt", true );

    CameraAspect* camera_aspect = m_cameraEntity.AddAspect<CameraAspect>();

    camera_aspect->AddComponent<Matrix>( "camera_proj" );

    DrawSpace::Interface::Renderer::Characteristics characteristics;
    m_renderer->GetRenderCharacteristics( characteristics );
    camera_aspect->GetComponent<Matrix>( "camera_proj" )->getPurpose().Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 100000.0 );

}

void MainService::OnKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':
        {
            TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();
            transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = 12.0;    
        }
        break;

        case 'W':
        {
            TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();
            transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = -12.0;
        }
        break;
    }
}

void MainService::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':
        case 'W':
        {
            TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();
            transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = 0.0; 
        }
        break;
    }
}

void MainService::OnKeyPulse( long p_key )
{
}

void MainService::OnChar( long p_char, long p_scan )
{
}

void MainService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();

    m_fps_yaw += - p_dx / 1.0;
    m_fps_pitch += - p_dy / 1.0;

    transform_aspect->GetComponent<dsreal>( "yaw" )->getPurpose() = m_fps_yaw.GetValue();
    transform_aspect->GetComponent<dsreal>( "pitch" )->getPurpose() = m_fps_pitch.GetValue();
}

void MainService::OnMouseWheel( long p_delta )
{
}

void MainService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
}

void MainService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
}

void MainService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
}

void MainService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
}

void MainService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void MainService::on_resource_event(DrawSpace::Systems::ResourcesSystem::ResourceEvent p_event, const dsstring& p_resource, const dsstring& p_context)
{
    if (p_event == DrawSpace::Systems::ResourcesSystem::ResourceEvent::ALL_ASSETS_LOADED)
    {
        m_systemsHub.GetSystem<Systems::ResourcesSystem>("ResourcesSystem").Deactivate();
        m_rendergraph.PushSignal_UpdatedRenderingQueues();

        if ("INIT" == p_context)
        {
            PhysicsAspect* physic_aspect{ m_world1Entity.GetAspect<PhysicsAspect>() };
            physic_aspect->RegisterCollider(&m_groundEntity);
            physic_aspect->RegisterRigidBody(&m_cubeEntity);
            physic_aspect->RegisterRigidBody(&m_cube2Entity);
            physic_aspect->RegisterRigidBody(&m_mainBodyEntity);            
        }
    }
}

void MainService::set_mouse_circular_mode( bool p_state )
{
    if( m_mousecircularmode_cb )
    {
        (*m_mousecircularmode_cb)( p_state );        
    }
}
