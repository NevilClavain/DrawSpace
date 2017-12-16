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

#include "mainservice.h"
#include "component.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

_DECLARE_DS_LOGGER( logger, "test04mainloopservice", NULL )

MainService::MainService( void ) :
m_waves_inc( true ),
m_hmi_mode( true ),
m_guiwidgetpushbuttonclicked_cb( this, &MainService::on_guipushbutton_clicked )
{
}

bool MainService::Init( void )
{
    m_systems.push_back( &m_timeSystem );
    m_systems.push_back( &m_physicsSystem );
    m_systems.push_back( &m_transformSystem );
    m_systems.push_back( &m_renderingSystem );

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

    logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( logconf );

    /////////////////////////////////////////////////////////////////////////////////

    for( size_t i = 0; i < m_systems.size(); i++ )
    {
        m_systems[i]->Init( &m_entitygraph );
    }

    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();


    m_renderer->GUI_RegisterPushButtonEventClickedHandler( &m_guiwidgetpushbuttonclicked_cb );

    /////////////////////////////////////////////////////////////////////////////////

    m_finalpass = m_rendergraph.CreateRoot( "final_pass" );

    m_finalpass.CreateViewportQuad();

    m_finalpass.GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    RenderStatesSet finalpass_rss;
    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "point" ) );
    finalpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    //finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    //finalpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    m_finalpass.GetViewportQuad()->GetFx()->SetRenderStates( finalpass_rss );


    m_finalpass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_mask.vso", true ) ) );
    m_finalpass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_mask.pso", true ) ) );

    m_finalpass.GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass.GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_finalpass.GetRenderingQueue()->EnableDepthClearing( false );
    m_finalpass.GetRenderingQueue()->EnableTargetClearing( false );


    m_texturepass = m_finalpass.CreateChild( "texture_pass", 0 );    
    m_texturepass.GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass.GetRenderingQueue()->EnableTargetClearing( false );


    m_texturemirrorpass = m_finalpass.CreateChild( "texturemirror_pass", 1 );
    m_texturemirrorpass.GetRenderingQueue()->EnableDepthClearing( true );
    m_texturemirrorpass.GetRenderingQueue()->EnableTargetClearing( false );


    m_bumppass = m_finalpass.CreateChild( "bump_pass", 2, Texture::RENDERPURPOSE_FLOATVECTOR );


    m_bumppass.GetRenderingQueue()->EnableDepthClearing( true );
    m_bumppass.GetRenderingQueue()->EnableTargetClearing( true );
    m_bumppass.GetRenderingQueue()->SetTargetClearingColor( 255, 255, 255, 255 );

    
    m_wavespass = m_finalpass.CreateChild( "wave_pass", RenderGraph::RenderPassNode::noTextureStageConnection, Core::Texture::RENDERPURPOSE_COLOR, Core::Texture::RENDERTARGET_GPU, false, 512, 512 );

    m_wavespass.CreateViewportQuad();

    m_wavespass.GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_wavespass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_waves.vso", true ) ) );
    m_wavespass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_waves.pso", true ) ) );
    m_wavespass.GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_wavespass.GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_wavespass.GetViewportQuad()->AddShaderParameter( 1, "waves", 0 );


    ////////////////////////////////////////////////////////////////////////////////////

    RenderingAspect* rendering_aspect = m_rootEntity.AddAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_passesRender );
    m_passesRender.SetRendergraph( &m_rendergraph );

    TimeAspect* time_aspect = m_rootEntity.AddAspect<TimeAspect>();

    time_aspect->AddComponent<TimeManager>( "time_manager" );
    time_aspect->AddComponent<TimeAspect::TimeScale>( "time_scale", TimeAspect::NORMAL_TIME );
    time_aspect->AddComponent<dsstring>( "output_formated_datetime", "..." );
    time_aspect->AddComponent<dstime>( "time", 0 );
    time_aspect->AddComponent<int>( "output_fps" );
    time_aspect->AddComponent<int>( "output_world_nbsteps" );

    time_aspect->AddComponent<dsreal>( "output_time_factor" );

    m_fps_yaw = time_aspect->TimeAngleFactory( 0.0 );
    m_fps_pitch = time_aspect->TimeAngleFactory( 0.0 );

    m_waves = time_aspect->TimeScalarFactory( 0.0 );

    m_rootEntityNode = m_entitygraph.SetRoot( &m_rootEntity );

    /////////////////////////////////////////////////////////////////////////////////////



    PhysicsAspect* physic_aspect = m_world1Entity.AddAspect<PhysicsAspect>();

    //physic_aspect->AddComponent<bool>( "gravity_state", true );
    physic_aspect->AddComponent<bool>( "gravity_state", false );
    physic_aspect->AddComponent<Vector>( "gravity", Vector( 0.0, -9.81, 0.0, 0.0 ) );

    //m_World1EntityNode = m_rootEntityNode.AddChild( &m_world1Entity );

    m_World1EntityNode = m_rootEntityNode.AddChild( &m_world1Entity );

    //////////////////////////////////////////////////////////////////////////////////////

    create_skybox();
    create_ground();
    create_static_cube();

    /////////////////////////////////////////////////////////////////////////////////////

    TransformAspect* transform_aspect = m_cameraEntity.AddAspect<TransformAspect>();

    transform_aspect->SetImplementation( &m_fps_transformer );
    transform_aspect->AddComponent<dsreal>( "yaw", 0.0 );
    transform_aspect->AddComponent<dsreal>( "pitch", 0.0 );

    transform_aspect->AddComponent<Vector>( "speed" );
    transform_aspect->AddComponent<Matrix>( "pos" );

    transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( 0.0, 1.0, 10.0, 1.0 ) );

    transform_aspect->AddComponent<bool>( "ymvt", true );

    CameraAspect* camera_aspect = m_cameraEntity.AddAspect<CameraAspect>();

    camera_aspect->AddComponent<Matrix>( "camera_proj" );

    DrawSpace::Interface::Renderer::Characteristics characteristics;
    m_renderer->GetRenderCharacteristics( characteristics );
    camera_aspect->GetComponent<Matrix>( "camera_proj" )->getPurpose().Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 100000.0 );

    camera_aspect->AddComponent<dsstring>( "camera_debug_name", "camera1 (fps)" );



    /////////////////////////////////////////////////////////////////////////////////////

    // ajouter la skybox a la scene
    m_skyboxEntityNode = m_rootEntityNode.AddChild( &m_skyboxEntity ); // comme la skybox n'a aucune interaction/influence avec le monde physique bullet, on peut la mettre directement sous rootEntity
                                                                        // mettre la skybox sous World1Entity fonctionne aussi, mais n'a aucune utilité
    m_skyboxRender->RegisterToRendering( m_rendergraph );


    // ajouter le ground a la scene
    m_groundEntityNode = m_World1EntityNode.AddChild( &m_groundEntity );
    m_groundRender.RegisterToRendering( m_rendergraph );


    // ajout static cube a la scene
    m_staticCubeEntityNode = m_World1EntityNode.AddChild( &m_staticCubeEntity );
    m_staticCubeRender.RegisterToRendering( m_rendergraph );

    // ajouter la cameras a la scene
    m_cameraEntityNode = m_rootEntityNode.AddChild( &m_cameraEntity );



    // designer la camera courante
    m_transformSystem.SetCurrentCameraEntity( &m_cameraEntity );



    ////////////////GUI ///////////////////////////////////////////////////////////////////


    m_renderer->GUI_InitSubSystem();
    m_renderer->GUI_SetResourcesRootDirectory( "./testskin" );
    
    m_renderer->GUI_LoadScheme( "AlfiskoSkin.scheme" );

    m_renderer->GUI_LoadLayout( "main.layout", "testskin/layouts/main_widgets.conf" );

    m_renderer->GUI_SetLayout( "main.layout" );


    m_rendergraph.RenderingQueueModSignal();
    m_entitygraph.OnSceneRenderBegin();



    set_mouse_circular_mode( !m_hmi_mode );

    _DSDEBUG( logger, dsstring("MainService : startup...") );

    return true;
}


void MainService::Run( void )
{
    for( size_t i = 0; i < m_systems.size(); i++ )
    {
        m_systems[i]->Run( &m_entitygraph );
    }

    m_renderer->GUI_Render();
    m_renderer->FlipScreen();

    if( m_waves_inc )
    {
        if( m_waves.GetValue() < 200.0 )
        {
            m_waves += 1.0;
        }
        else
        {
            m_waves_inc = false;
        }
    }
    else
    {
        if( m_waves.GetValue() > 0.0 )
        {
            m_waves -= 1.0;
        }
        else
        {
            m_waves_inc = true;
        }        
    }

    m_wavespass.GetViewportQuad()->SetShaderRealVector( "waves", DrawSpace::Utils::Vector( m_waves.GetValue(), 0.0, 0.0, 0.0 ) );

}

void MainService::Release( void )
{
    _DSDEBUG( logger, dsstring("MainService : shutdown...") );

    m_entitygraph.OnSceneRenderEnd();
    for( size_t i = 0; i < m_systems.size(); i++ )
    {
        m_systems[i]->Release( &m_entitygraph );
    }
}

void MainService::OnKeyPress( long p_key )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnKeyUp( p_key );
        return;
    }

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
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnKeyDown( p_key );
        return;
    }

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
    switch( p_key )
    {
        case VK_F1:

            m_hmi_mode = !m_hmi_mode;
            set_mouse_circular_mode( !m_hmi_mode );            
            break;
    }
}

void MainService::OnChar( long p_char, long p_scan )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnChar( p_char );
        return;
    }
}

void MainService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( !m_hmi_mode )
    {
        TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();

        m_fps_yaw += - p_dx / 1.0;
        m_fps_pitch += - p_dy / 1.0;

        transform_aspect->GetComponent<dsreal>( "yaw" )->getPurpose() = m_fps_yaw.GetValue();
        transform_aspect->GetComponent<dsreal>( "pitch" )->getPurpose() = m_fps_pitch.GetValue();
    }
    else
    {
        m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
    }
}

void MainService::OnMouseWheel( long p_delta )
{
}

void MainService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnMouseLeftButtonDown();
    }
}

void MainService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnMouseLeftButtonUp();
    }
}

void MainService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnMouseRightButtonDown();
    }
}

void MainService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnMouseRightButtonUp();
    }
}

void MainService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void MainService::create_skybox( void )
{    
    DrawSpace::Interface::Module::Root* sbmod_root;

    if( !DrawSpace::Utils::PILoad::LoadModule( "skyboxmod", "skybox", &sbmod_root ) )
    {
        _DSEXCEPTION( "fail to load skyboxmod module root" )
    }
    m_skyboxRender = sbmod_root->InstanciateRenderingAspectImpls( "skyboxRender" );


    RenderingAspect* rendering_aspect = m_skyboxEntity.AddAspect<RenderingAspect>();
    
    rendering_aspect->AddImplementation( m_skyboxRender );

    ////////////// noms des passes

    std::vector<dsstring> skybox_passes;

    skybox_passes.push_back( "texture_pass" );
    skybox_passes.push_back( "texturemirror_pass" );

    rendering_aspect->AddComponent<std::vector<dsstring>>( "skybox_passes", skybox_passes );

    ////////////// jeu de 6 textures par slot pass

    std::vector<Texture*> skybox_textures;
    skybox_textures.push_back( _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) ) );
    skybox_textures.push_back( _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) ) );
    skybox_textures.push_back( _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) ) );
    skybox_textures.push_back( _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) ) );
    skybox_textures.push_back( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ) );
    skybox_textures.push_back( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ) );

    for( int i = 0; i < 6; i++ )
    {
        skybox_textures[i]->LoadFromFile();    
    }

    rendering_aspect->AddComponent<std::vector<Texture*>>( "skybox_textures", skybox_textures );
    rendering_aspect->AddComponent<std::vector<Texture*>>( "skybox_mirror_textures", skybox_textures );

    /////////////// les FX pour chaque slot pass

    Fx* skybox_texturepass_fx = _DRAWSPACE_NEW_( Fx, Fx );

    skybox_texturepass_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    skybox_texturepass_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    skybox_texturepass_fx->GetShader( 0 )->LoadFromFile();
    skybox_texturepass_fx->GetShader( 1 )->LoadFromFile();

    RenderStatesSet skybox_texturepass_rss;
    skybox_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    skybox_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    skybox_texturepass_fx->SetRenderStates( skybox_texturepass_rss );

    rendering_aspect->AddComponent<Fx*>( "skybox_fx", skybox_texturepass_fx );

    
    Fx* skybox_texturemirrorpass_fx = _DRAWSPACE_NEW_( Fx, Fx );

    skybox_texturemirrorpass_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
    skybox_texturemirrorpass_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );

    skybox_texturemirrorpass_fx->GetShader( 0 )->LoadFromFile();
    skybox_texturemirrorpass_fx->GetShader( 1 )->LoadFromFile();

    RenderStatesSet skybox_texturemirrorpass_rss;
    skybox_texturemirrorpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    skybox_texturemirrorpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
    skybox_texturemirrorpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    skybox_texturemirrorpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    skybox_texturemirrorpass_fx->SetRenderStates( skybox_texturemirrorpass_rss );
    
    rendering_aspect->AddComponent<Fx*>( "skybox_mirror_fx", skybox_texturemirrorpass_fx );
    

    /////////// params shaders
    
    //std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>> skybox_shaders_params;

    std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> skybox_texturepass_shaders_params;

    
    std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> skybox_texturemirrorpass_shaders_params;

    std::pair<dsstring, RenderingNode::ShadersParams> reflector_pos;
    reflector_pos.first = "reflector_pos";
    reflector_pos.second.shader_index = 0;
    reflector_pos.second.param_register = 24;
    reflector_pos.second.vector = true;
    reflector_pos.second.param_values = Vector( 0.0, -4.0, 0.0, 1.0 );


    std::pair<dsstring, RenderingNode::ShadersParams> reflector_normale;
    reflector_normale.first = "reflector_normale";
    reflector_normale.second.shader_index = 0;
    reflector_normale.second.param_register = 25;
    reflector_normale.second.vector = true;
    reflector_normale.second.param_values = Vector( 0.0, 1.0, 0.0, 1.0 );
    

    skybox_texturemirrorpass_shaders_params.push_back( reflector_pos );
    skybox_texturemirrorpass_shaders_params.push_back( reflector_normale );
    

    //skybox_shaders_params.push_back( skybox_texturepass_shaders_params );
    //skybox_shaders_params.push_back( skybox_texturemirrorpass_shaders_params );

    //rendering_aspect->AddComponent<std::vector<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>>( "skybox_shaders_params", skybox_shaders_params );

    rendering_aspect->AddComponent<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>( "skybox_texturepass_shaders_params", skybox_texturepass_shaders_params );
    rendering_aspect->AddComponent<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>( "skybox_texturemirrorpass_shaders_params", skybox_texturemirrorpass_shaders_params );

    //////////////// valeur du rendering order pour chaque slot pass
    rendering_aspect->AddComponent<int>( "skybox_ro", -1000 );
    rendering_aspect->AddComponent<int>( "skybox_mirror_ro", -1000 );


    TransformAspect* transform_aspect = m_skyboxEntity.AddAspect<TransformAspect>();

    transform_aspect->SetImplementation( &m_skybox_transformer );

    transform_aspect->AddComponent<Matrix>( "skybox_scaling" );

    transform_aspect->GetComponent<Matrix>( "skybox_scaling" )->getPurpose().Scale( 100.0, 100.0, 100.0 );
    
}

void MainService::create_static_cube( void )
{
    RenderingAspect* rendering_aspect = m_staticCubeEntity.AddAspect<RenderingAspect>();
    rendering_aspect->AddImplementation( &m_staticCubeRender );


    rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( "texturepass_slot", "texture_pass" );

    RenderingNode* cube_texturepass = rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( "texturepass_slot" )->getPurpose().GetRenderingNode();
    cube_texturepass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    cube_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    cube_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );


    cube_texturepass->GetFx()->GetShader( 0 )->LoadFromFile();
    cube_texturepass->GetFx()->GetShader( 1 )->LoadFromFile();

    cube_texturepass->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "002b2su2.jpg" ) ), 0 );
    cube_texturepass->GetTexture( 0 )->LoadFromFile();


    RenderStatesSet cube_texturepass_rss;
    cube_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    cube_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    cube_texturepass->GetFx()->SetRenderStates( cube_texturepass_rss );


    cube_texturepass->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    cube_texturepass->GetMeshe()->SetImporter( m_meshe_import );
    cube_texturepass->GetMeshe()->LoadFromFile( "object.ac", 0 );

    //////////////////////////////////////////////////////////////////////////

    rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( "texturemirrorpass_slot", "texturemirror_pass" );

    RenderingNode* cube_texturemirrorpass = rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( "texturemirrorpass_slot" )->getPurpose().GetRenderingNode();
    cube_texturemirrorpass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    cube_texturemirrorpass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
    cube_texturemirrorpass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );


    cube_texturemirrorpass->GetFx()->GetShader( 0 )->LoadFromFile();
    cube_texturemirrorpass->GetFx()->GetShader( 1 )->LoadFromFile();

    cube_texturemirrorpass->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "002b2su2.jpg" ) ), 0 );
    cube_texturemirrorpass->GetTexture( 0 )->LoadFromFile();


    RenderStatesSet cube_texturemirrorpass_rss;
    cube_texturemirrorpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    cube_texturemirrorpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    cube_texturemirrorpass->GetFx()->SetRenderStates( cube_texturemirrorpass_rss );


    cube_texturemirrorpass->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    cube_texturemirrorpass->GetMeshe()->SetImporter( m_meshe_import );
    cube_texturemirrorpass->GetMeshe()->LoadFromFile( "object.ac", 0 );

    cube_texturemirrorpass->AddShaderParameter( 0, "reflector_pos", 24 );
    cube_texturemirrorpass->AddShaderParameter( 0, "reflector_normale", 25 );

    cube_texturemirrorpass->SetShaderRealVector( "reflector_pos", Vector( 0.0, -4.0, 0.0, 1.0 ) );
    cube_texturemirrorpass->SetShaderRealVector( "reflector_normale", Vector( 0.0, 1.0, 0.0, 1.0 ) );

    //////////////////////////////////////////////////////////////////////////


   TransformAspect* transform_aspect = m_staticCubeEntity.AddAspect<TransformAspect>();


    BodyAspect* body_aspect = m_staticCubeEntity.AddAspect<BodyAspect>();


    body_aspect->AddComponent<BodyAspect::BoxCollisionShape>( "shape", Vector( 0.5, 0.5, 0.5, 1.0 ) );


    Matrix cube_attitude;
    
    cube_attitude.Translation( 0.8, 1.1, 0.3 );

    body_aspect->AddComponent<Matrix>( "attitude", cube_attitude );

    
    body_aspect->AddComponent<BodyAspect::Mode>( "mode", BodyAspect::COLLIDER );

    body_aspect->AddComponent<bool>( "enable", true );
    body_aspect->AddComponent<bool>( "contact_state", false );

    transform_aspect->SetImplementation( body_aspect->GetTransformAspectImpl() );

}

void MainService::create_ground( void )
{
    RenderingAspect* rendering_aspect = m_groundEntity.AddAspect<RenderingAspect>();
    rendering_aspect->AddImplementation( &m_groundRender );


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( "texturepass_slot", "texture_pass" );

    
    RenderingNode* ground_texturepass = rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( "texturepass_slot" )->getPurpose().GetRenderingNode();
    ground_texturepass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    ////////////////////////////////////////////////////////

    ground_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.vso", true ) ) );
    ground_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.pso", true ) ) );

    ////////////////////////////////////////////////////////

    //ground_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    //ground_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );


    ////////////////////////////////////////////////////////



    ground_texturepass->GetFx()->GetShader( 0 )->LoadFromFile();
    ground_texturepass->GetFx()->GetShader( 1 )->LoadFromFile();

    ground_texturepass->AddShaderParameter( 1, "color", 0 );
    ground_texturepass->SetShaderRealVector( "color", Vector( 1.0, 0.0, 1.0, 1.0 ) );


    RenderStatesSet ground_texturepass_rss;
    ground_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    ground_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    ground_texturepass->GetFx()->SetRenderStates( ground_texturepass_rss );


    ground_texturepass->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    ground_texturepass->GetMeshe()->SetImporter( m_meshe_import );
    ground_texturepass->GetMeshe()->LoadFromFile( "water.ac", 0 );

    //ground_texturepass->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "002b2su2.jpg" ) ), 0 );
    //ground_texturepass->GetTexture( 0 )->LoadFromFile();
    
    //ground_texturepass->SetTexture( m_wavespass.GetTargetTexture(), 0 );


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( "bumppass_slot", "bump_pass" );

    RenderingNode* ground_bumppass = rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( "bumppass_slot" )->getPurpose().GetRenderingNode();

    ground_bumppass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    ground_bumppass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_bump.vso", true ) ) );
    ground_bumppass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_bump.pso", true ) ) );
    ground_bumppass->GetFx()->GetShader( 0 )->LoadFromFile();
    ground_bumppass->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet ground_bumppass_rss;

    ground_bumppass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    ground_bumppass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    ground_bumppass->GetFx()->SetRenderStates( ground_bumppass_rss );


    ground_bumppass->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    ground_bumppass->GetMeshe()->SetImporter( m_meshe_import );
    ground_bumppass->GetMeshe()->LoadFromFile( "water.ac", 0 );

    ground_bumppass->SetTexture( m_wavespass.GetTargetTexture(), 0 );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TransformAspect* transform_aspect = m_groundEntity.AddAspect<TransformAspect>();


    BodyAspect* body_aspect = m_groundEntity.AddAspect<BodyAspect>();


    body_aspect->AddComponent<BodyAspect::BoxCollisionShape>( "shape", Vector( 100.0, 0.0, 100.0, 1.0 ) );


    Matrix ground_attitude;
    
    //ground_attitude.Identity();
    ground_attitude.Translation( 0.0, -4.0, 0.0 );

    body_aspect->AddComponent<Matrix>( "attitude", ground_attitude );

    
    body_aspect->AddComponent<BodyAspect::Mode>( "mode", BodyAspect::COLLIDER );

    body_aspect->AddComponent<bool>( "enable", true );
    body_aspect->AddComponent<bool>( "contact_state", false );

    transform_aspect->SetImplementation( body_aspect->GetTransformAspectImpl() );

}


void MainService::set_mouse_circular_mode( bool p_state )
{
    if( m_mousecircularmode_cb )
    {
        (*m_mousecircularmode_cb)( p_state );

        m_renderer->GUI_ShowMouseCursor( !p_state );
    }
}

void MainService::on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id )
{
    if( "Trigger" == p_widget_id )
    {
        _asm nop
    }
    else if( "Quit" == p_widget_id )
    {
        (*m_closeapp_cb)( 0 );
    }
    else if( "Button_Create" == p_widget_id )
    {
        _asm nop
    }
    else if( "Button_Destroy" == p_widget_id )
    {
        _asm nop
    }
}