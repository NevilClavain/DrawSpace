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

#include "mainloopservice.h"
#include "drawspace.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "test01mainloopservice", NULL )

MainLoopService::MainLoopService( void ) :
m_fps_transformer( m_tm ),
m_free_transformer( m_tm ),
m_left_mousebutton( false ),
m_right_mousebutton( false ),
m_current_camera( 0 ),
m_worldsystem_evt_handler( this, &MainLoopService::on_transformsystem_evt ),
m_entitygraph_evt_handler( this, &MainLoopService::on_entitygraph_evt ),
m_show_cube( true )
{
}

MainLoopService::~MainLoopService( void )
{
}

void MainLoopService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{

}

void MainLoopService::Init( DrawSpace::Logger::Configuration* p_logconf, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                            DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{
    m_roty = 0.0;

    m_entitygraph.RegisterNodesEvtHandler( &m_entitygraph_evt_handler );

    (*p_mousecircularmode_cb)( true )
        ;
    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();

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


    m_finalpass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_finalpass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_finalpass.GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass.GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_finalpass.GetRenderingQueue()->EnableDepthClearing( false );
    m_finalpass.GetRenderingQueue()->EnableTargetClearing( false );




    m_texturepass = m_finalpass.CreateChild( "texture_pass", 0 );

    //m_texturepass.GetRenderingQueue()->SetTargetClearingColor( 0, 0, 200, 255 );
    m_texturepass.GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass.GetRenderingQueue()->EnableTargetClearing( false );


    //////////////////////////////////////////////////////////////////////////
    
    RenderingAspect* rendering_aspect = m_rootEntity.AddAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_passesRender );

    m_passesRender.SetRendergraph( &m_rendergraph );

    rendering_aspect->AddImplementation( &m_textRender );
    rendering_aspect->AddComponent<TextRenderingAspectImpl::TextDisplay>( "fps", 10, 10, 0, 255, 0, "..." );
    rendering_aspect->AddComponent<TextRenderingAspectImpl::TextDisplay>( "current camera", 10, 30, 255, 0, 255, "..." );


    m_rootEntityNode = m_entitygraph.SetRoot( &m_rootEntity );
    

    //////////////////////////////////////////////////////////////////////////

    PhysicsAspect* physic_aspect = m_world1Entity.AddAspect<PhysicsAspect>();

    //physic_aspect->AddComponent<bool>( "gravity_state", true );
    physic_aspect->AddComponent<bool>( "gravity_state", false );
    physic_aspect->AddComponent<Vector>( "gravity", Vector( 0.0, -9.81, 0.0, 0.0 ) );

    m_World1EntityNode = m_rootEntityNode.AddChild( &m_world1Entity );

    //////////////////////////////////////////////////////////////////////////

    physic_aspect = m_world2Entity.AddAspect<PhysicsAspect>();

    //physic_aspect->AddComponent<bool>( "gravity_state", true );
    physic_aspect->AddComponent<bool>( "gravity_state", false );
    physic_aspect->AddComponent<Vector>( "gravity", Vector( 0.0, -9.81, 0.0, 0.0 ) );

    m_World2EntityNode = m_rootEntityNode.AddChild( &m_world2Entity );

    //////////////////////////////////////////////////////////////////////////


    Matrix cube_transf;

    cube_transf.Translation( 0.0, 10.0, -10.0 );
    create_cube( cube_transf );

    Matrix sphere_transf;

    sphere_transf.Translation( 0.0, 10.0, 10.0 );

    
    create_sphere( sphere_transf );

    create_ground();

    ///////////////////////////////////////////////////////////////////////////

    TransformAspect* transform_aspect = m_cameraEntity.AddAspect<TransformAspect>();

    transform_aspect->AddImplementation( &m_fps_transformer );
    transform_aspect->AddComponent<dsreal>( "yaw", 0.0 );
    transform_aspect->AddComponent<dsreal>( "pitch", 0.0 );

    transform_aspect->AddComponent<Vector>( "speed" );
    transform_aspect->AddComponent<Matrix>( "pos" );

    transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( -8.0, 12.0, 5.0, 1.0 ) );

    transform_aspect->AddComponent<bool>( "ymvt", true );

    CameraAspect* camera_aspect = m_cameraEntity.AddAspect<CameraAspect>();

    camera_aspect->AddComponent<Matrix>( "camera_proj" );

    DrawSpace::Interface::Renderer::Characteristics characteristics;
    m_renderer->GetRenderCharacteristics( characteristics );
    camera_aspect->GetComponent<Matrix>( "camera_proj" )->getPurpose().Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 100000.0 );

    camera_aspect->AddComponent<dsstring>( "camera_debug_name", "camera1 (fps)" );


    ///////////////////////////////////////////////////////////////////////////

    transform_aspect = m_camera2Entity.AddAspect<TransformAspect>();

    transform_aspect->AddImplementation( &m_free_transformer );
  
    transform_aspect->AddComponent<dsreal>( "rspeed_x", 0.0 );
    transform_aspect->AddComponent<dsreal>( "rspeed_y", 0.0 );
    transform_aspect->AddComponent<dsreal>( "rspeed_z", 0.0 );

    transform_aspect->AddComponent<Vector>( "speed" );
    transform_aspect->AddComponent<Vector>( "rot_axis_x", Vector( 1.0, 0.0, 0.0, 1.0 ) );
    transform_aspect->AddComponent<Vector>( "rot_axis_y", Vector( 0.0, 1.0, 0.0, 1.0 ) );
    transform_aspect->AddComponent<Vector>( "rot_axis_z", Vector( 0.0, 0.0, 1.0, 1.0 ) );

    transform_aspect->AddComponent<Matrix>( "pos" );
    transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( 0.0, 2.0, 5.0, 1.0 ) );

    transform_aspect->AddComponent<Quaternion>( "quat" );
    transform_aspect->GetComponent<Quaternion>( "quat" )->getPurpose().Identity();
    


    camera_aspect = m_camera2Entity.AddAspect<CameraAspect>();

    camera_aspect->AddComponent<Matrix>( "camera_proj" );

    m_renderer->GetRenderCharacteristics( characteristics );
    camera_aspect->GetComponent<Matrix>( "camera_proj" )->getPurpose().Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 100000.0 );

    camera_aspect->AddComponent<dsstring>( "camera_debug_name", "camera2 (free)" );

    ///////////////////////////////////////////////////////////////////////////

    create_skybox();

    ///////////////////////////////////////////////////////////////////////////

    m_transformSystem.RegisterCameraEvtHandler( &m_worldsystem_evt_handler );


    // ajouter la skybox a la scene
    m_skyboxEntityNode = m_rootEntityNode.AddChild( &m_skyboxEntity ); // comme la skybox n'a aucune interaction/influence avec le monde physique bullet, on peut la mettre directement sous rootEntity
                                                                        // mettre la skybox sous World1Entity fonctionne aussi, mais n'a aucune utilité
    m_skyboxRender.RegisterToRendering( m_rendergraph );

    // ajouter le cube a la scene
    m_cubeEntityNode = m_World1EntityNode.AddChild( &m_cubeEntity );
    m_cubeRender.RegisterToRendering( m_rendergraph );

    // ajouter la sphere a la scene
    /*
    m_sphereEntityNode = m_World1EntityNode.AddChild( &m_sphereEntity );
    */
    m_sphereEntityNode = m_World2EntityNode.AddChild( &m_sphereEntity );
    m_sphereRender.RegisterToRendering( m_rendergraph );

    // ajouter le ground a la scene
    m_groundEntityNode = m_World1EntityNode.AddChild( &m_groundEntity );
    m_groundRender.RegisterToRendering( m_rendergraph );


    // ajouter la camera a la scene
    m_cameraEntityNode = m_World1EntityNode.AddChild( &m_cameraEntity );

    m_camera2EntityNode = m_World1EntityNode.AddChild( &m_camera2Entity );


    if( 0 == m_current_camera )
    {
        m_transformSystem.SetCurrentCameraEntity( &m_cameraEntity );
    }
    else
    {
        m_transformSystem.SetCurrentCameraEntity( &m_camera2Entity );
    }

    m_rendergraph.RenderingQueueModSignal();

    m_physicsSystem.SetTimeManager( &m_tm );

    _DSDEBUG( logger, dsstring("main loop service : startup...") );
}

void MainLoopService::Run( void )
{
    m_physicsSystem.Run( &m_entitygraph );
    m_transformSystem.Run( &m_entitygraph );
    m_renderingSystem.Run( &m_entitygraph );
    m_renderer->FlipScreen();

    if( m_display_switch )
    {      
        char comment[256];
        sprintf( comment, "%d fps - %s", m_tm.GetFPS(), m_pluginDescr.c_str() );

        RenderingAspect* rendering_aspect = m_rootEntity.GetAspect<RenderingAspect>();

        rendering_aspect->GetComponent<TextRenderingAspectImpl::TextDisplay>( "fps" )->getPurpose().m_text = comment;
    }

    m_tm.Update();
    if( m_tm.IsReady() )
    {

        
        m_tm.AngleSpeedInc( &m_roty, 15 );

        BodyAspect* body_aspect = m_sphereEntity.GetAspect<BodyAspect>();

        Matrix planet_rot;

        planet_rot.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Utils::Maths::DegToRad( m_roty ) );

        Matrix planet_transf;
        planet_transf.Translation( 0.0, 10.0, 10.0 );

        Matrix planet_mat = planet_rot * planet_transf;

        body_aspect->GetComponent<Matrix>( "attitude" )->getPurpose() = planet_mat;
    }
    
    if( 1 == m_current_camera )
    {
        TransformAspect* transform_aspect = m_camera2Entity.GetAspect<TransformAspect>();
        transform_aspect->GetComponent<dsreal>( "rspeed_x" )->getPurpose() = 0.0;
        transform_aspect->GetComponent<dsreal>( "rspeed_y" )->getPurpose() = 0.0;
        transform_aspect->GetComponent<dsreal>( "rspeed_z" )->getPurpose() = 0.0;
    }
}

void MainLoopService::Release( void )
{
    _DSDEBUG( logger, dsstring("main loop service : shutdown...") );

}

DrawSpace::Core::BaseSceneNode* MainLoopService::InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler )
{
    return NULL;
}

void MainLoopService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void MainLoopService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{ 
}

void MainLoopService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
}

void MainLoopService::OnKeyPress( long p_key )
{   
    switch( p_key )
    {
        case 'Q':
        {
            if( 0 == m_current_camera )
            {
                TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();
                transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = 2.0; 
            }
            else
            {
                TransformAspect* transform_aspect = m_camera2Entity.GetAspect<TransformAspect>();
                transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = 2.0;    
            }
        }
        break;

        case 'W':
        {
            if( 0 == m_current_camera )
            {
                TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();
                transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = -2.0;
            }
            else
            {
                TransformAspect* transform_aspect = m_camera2Entity.GetAspect<TransformAspect>();
                transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = -2.0;    
            }
        }
        break;

        case 'A':
        {
            BodyAspect* body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
            body_aspect->GetComponent<BodyAspect::Force>( "up_force" )->getPurpose().Enable();
        }
        break;

        case 'Z':
        {
            BodyAspect* body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
            body_aspect->GetComponent<BodyAspect::Torque>( "torque" )->getPurpose().Enable();        
        }
        break;

        case 'E':
        {
            BodyAspect* body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
            body_aspect->GetComponent<BodyAspect::Torque>( "torque_neg" )->getPurpose().Enable();        
        }
        break;


    }
}

void MainLoopService::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':
        case 'W':
        {
            if( 0 == m_current_camera )
            {
                TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();
                transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = 0.0; 
            }
            else
            {
                TransformAspect* transform_aspect = m_camera2Entity.GetAspect<TransformAspect>();
                transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = 0.0;             
            }
        }
        break;  

        case 'A':
        {
            BodyAspect* body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
            body_aspect->GetComponent<BodyAspect::Force>( "up_force" )->getPurpose().Disable();
        }
        break;

        case 'Z':
        {
            BodyAspect* body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
            body_aspect->GetComponent<BodyAspect::Torque>( "torque" )->getPurpose().Disable();        
        }
        break;

        case 'E':
        {
            BodyAspect* body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
            body_aspect->GetComponent<BodyAspect::Torque>( "torque_neg" )->getPurpose().Disable();        
        
        }
        break;
    }
}

void MainLoopService::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:
            {
                
                if( m_display_switch )
                {
                    RenderingAspect* renderingAspect = m_rootEntity.GetAspect<RenderingAspect>();
                    renderingAspect->RemoveComponent<TextRenderingAspectImpl::TextDisplay>( "fps");

                    m_display_switch = false;
                }
                else
                {
                    RenderingAspect* renderingAspect = m_rootEntity.GetAspect<RenderingAspect>();
                    renderingAspect->AddComponent<TextRenderingAspectImpl::TextDisplay>( "fps", 10, 10, 0, 255, 0, "..." );

                    m_display_switch = true;
                }
                
            }       
            break;

        case VK_F2:
            {
                if( 0 == m_current_camera )
                {
                    m_current_camera = 1;
                }
                else
                {
                    m_current_camera = 0;
                }

                if( 0 == m_current_camera )
                {
                    m_transformSystem.SetCurrentCameraEntity( &m_cameraEntity );
                }
                else
                {
                    m_transformSystem.SetCurrentCameraEntity( &m_camera2Entity );
                }
            }
            break;

        case VK_F3:
            {
                if( m_show_cube )
                {
                    m_show_cube = false;

                    m_cubeEntityNode.Erase();
                    m_cubeRender.UnregisterFromRendering( m_rendergraph );
                    m_rendergraph.RenderingQueueModSignal();

                }
                else
                {
                    m_show_cube = true;

                    m_cubeEntityNode = m_World1EntityNode.AddChild( &m_cubeEntity );
                    m_cubeRender.RegisterToRendering( m_rendergraph );
                    m_rendergraph.RenderingQueueModSignal();

                }
            }
            break;

        case VK_F4:
            {
                PhysicsAspect* physic_aspect = m_rootEntity.GetAspect<PhysicsAspect>();

                if( physic_aspect->GetComponent<bool>( "gravity_state" )->getPurpose() )
                {
                    physic_aspect->GetComponent<bool>( "gravity_state" )->getPurpose() = false;
                }
                else
                {
                    physic_aspect->GetComponent<bool>( "gravity_state" )->getPurpose() = true;
                }

            }
            break;
    }
}

void MainLoopService::OnChar( long p_char, long p_scan )
{
}

void MainLoopService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( 0 == m_current_camera )
    {
        TransformAspect* transform_aspect = m_cameraEntity.GetAspect<TransformAspect>();

        if( m_left_mousebutton )
        {
            m_tm.AngleSpeedInc( &transform_aspect->GetComponent<dsreal>( "yaw" )->getPurpose(), - p_dx / 4.0 );
            m_tm.AngleSpeedInc( &transform_aspect->GetComponent<dsreal>( "pitch" )->getPurpose(), - p_dy / 4.0 );
        }
    }
    else
    {
        TransformAspect* transform_aspect = m_camera2Entity.GetAspect<TransformAspect>();

        if( m_left_mousebutton )
        {
            transform_aspect->GetComponent<dsreal>( "rspeed_x" )->getPurpose() = - p_dy / 4.0;
            transform_aspect->GetComponent<dsreal>( "rspeed_y" )->getPurpose() = - p_dx / 4.0;
        }
        else if( m_right_mousebutton )
        {
            transform_aspect->GetComponent<dsreal>( "rspeed_z" )->getPurpose() = - p_dx;
        }
    }
}

void MainLoopService::OnMouseWheel( long p_delta )
{
}


void MainLoopService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_left_mousebutton = true;
}

void MainLoopService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_left_mousebutton = false;
}

void MainLoopService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_right_mousebutton = true;
}

void MainLoopService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_right_mousebutton = false;
}

void MainLoopService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void MainLoopService::on_transformsystem_evt( Systems::TransformSystem::Event p_evt, Core::Entity* p_entity )
{
    if( Systems::TransformSystem::CAMERA_ACTIVE == p_evt && p_entity )
    {
        CameraAspect* curr_camera_aspect = p_entity->GetAspect<CameraAspect>();
        RenderingAspect* rendering_aspect = m_rootEntity.GetAspect<RenderingAspect>();
    
        // mise a jour affichage avec le nom de la camera courante...
        rendering_aspect->GetComponent<dsstring>( "current camera" )->getPurpose() = curr_camera_aspect->GetComponent<dsstring>( "camera_debug_name" )->getPurpose();
    }
    else if( Systems::TransformSystem::RUN_BEGIN == p_evt )
    {
        _asm nop
    }
    else if( Systems::TransformSystem::RUN_END == p_evt )
    {
        _asm nop
    }
}

void MainLoopService::on_entitygraph_evt( DrawSpace::EntityGraph::EntityNode::Event p_evt, DrawSpace::Core::Entity* p_entity )
{

}

void MainLoopService::create_cube( const Matrix& p_transform )
{
    RenderingAspect* rendering_aspect = m_cubeEntity.AddAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_cubeRender );

    rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( "cube_texturepass_slot", "texture_pass" );

    
    RenderingNode* cube_texturepass = rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( "cube_texturepass_slot" )->getPurpose().GetRenderingNode();
    cube_texturepass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    cube_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    cube_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    cube_texturepass->GetFx()->GetShader( 0 )->LoadFromFile();
    cube_texturepass->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet cube_texturepass_rss;
    cube_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    cube_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    cube_texturepass->GetFx()->SetRenderStates( cube_texturepass_rss );


    cube_texturepass->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    cube_texturepass->GetMeshe()->SetImporter( m_meshe_import );
    cube_texturepass->GetMeshe()->LoadFromFile( "object.ac", 0 );

    cube_texturepass->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    cube_texturepass->GetTexture( 0 )->LoadFromFile();

    m_cubeEntity.AddAspect<TransformAspect>();
    
    BodyAspect* body_aspect = m_cubeEntity.AddAspect<BodyAspect>();

    body_aspect->AddComponent<BodyAspect::BoxCollisionShape>( "shape", Vector( 0.5, 0.5, 0.5, 1.0 ) );

    body_aspect->AddComponent<Matrix>( "attitude", p_transform );

    body_aspect->AddComponent<dsreal>( "mass", 7.0 );

    body_aspect->AddComponent<BodyAspect::Force>( "up_force", Vector( 0.0, 0.0, 90.0, 1.0 ) );

    body_aspect->AddComponent<BodyAspect::Torque>( "torque", Vector( 0.0, 7.0, 0.0, 1.0 ) );
    body_aspect->AddComponent<BodyAspect::Torque>( "torque_neg", Vector( 0.0, -7.0, 0.0, 1.0 ) );

    //body_aspect->AddComponent<bool>( "enable", true );

    TransformAspect* transform_aspect = m_cubeEntity.GetAspect<TransformAspect>();
    transform_aspect->AddImplementation( body_aspect->GetTransformAspectImpl() );
    
}

void MainLoopService::create_skybox( void )
{
    RenderingAspect* rendering_aspect = m_skyboxEntity.AddAspect<RenderingAspect>();
    

    rendering_aspect->AddImplementation( &m_skyboxRender );

    rendering_aspect->AddComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot", "texture_pass" );

    Fx* skybox_texturepass_fx = _DRAWSPACE_NEW_( Fx, Fx );

    skybox_texturepass_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    skybox_texturepass_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    skybox_texturepass_fx->GetShader( 0 )->LoadFromFile();
    skybox_texturepass_fx->GetShader( 1 )->LoadFromFile();

    RenderStatesSet skybox_texturepass_rss;
    skybox_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    skybox_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    skybox_texturepass_fx->SetRenderStates( skybox_texturepass_rss );

    for( int i = 0; i < 6; i++ )
    {
        RenderingNode* skybox_texturepass = rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( i );
        skybox_texturepass->SetOrderNumber( -1000 );
        skybox_texturepass->SetFx( skybox_texturepass_fx );
    }

    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) ), 0 );
    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) ), 0 );
    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::RearQuad )->GetTexture( 0 )->LoadFromFile();

    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) ), 0 );
    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) ), 0 );
    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::RightQuad )->GetTexture( 0 )->LoadFromFile();

    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::TopQuad )->GetTexture( 0 )->LoadFromFile();

    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    rendering_aspect->GetComponent<SkyboxRenderingAspectImpl::PassSlot>( "skybox_texturepass_slot" )->getPurpose().GetRenderingNode( SkyboxRenderingAspectImpl::PassSlot::BottomQuad )->GetTexture( 0 )->LoadFromFile();


    TransformAspect* transform_aspect = m_skyboxEntity.AddAspect<TransformAspect>();

    transform_aspect->AddImplementation( &m_transformer );

    transform_aspect->AddComponent<Matrix>( "skybox_scaling" );

    transform_aspect->GetComponent<Matrix>( "skybox_scaling" )->getPurpose().Scale( 100.0, 100.0, 100.0 );
}

void MainLoopService::create_ground( void )
{
    RenderingAspect* rendering_aspect = m_groundEntity.AddAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_groundRender );

    rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( "texturepass_slot", "texture_pass" );

    
    RenderingNode* ground_texturepass = rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( "texturepass_slot" )->getPurpose().GetRenderingNode();
    ground_texturepass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    ground_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    ground_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    ground_texturepass->GetFx()->GetShader( 0 )->LoadFromFile();
    ground_texturepass->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet ground_texturepass_rss;
    ground_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    ground_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    ground_texturepass->GetFx()->SetRenderStates( ground_texturepass_rss );


    ground_texturepass->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    ground_texturepass->GetMeshe()->SetImporter( m_meshe_import );
    ground_texturepass->GetMeshe()->LoadFromFile( "water.ac", 0 );

    ground_texturepass->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "002b2su2.jpg" ) ), 0 );
    ground_texturepass->GetTexture( 0 )->LoadFromFile();

    TransformAspect* transform_aspect = m_groundEntity.AddAspect<TransformAspect>();


    BodyAspect* body_aspect = m_groundEntity.AddAspect<BodyAspect>();


    //body_aspect->AddComponent<BodyAspect::Shape>( "shape", BodyAspect::BOX_SHAPE );
    //body_aspect->AddComponent<Vector>( "shape_box_dims", Vector( 100.0, 0.0, 100.0, 1.0 ) );

    body_aspect->AddComponent<BodyAspect::BoxCollisionShape>( "shape", Vector( 100.0, 0.0, 100.0, 1.0 ) );


    Matrix ground_attitude;
    
    ground_attitude.Identity();

    body_aspect->AddComponent<Matrix>( "attitude", ground_attitude );


    body_aspect->AddComponent<bool>( "collider", true );
    //body_aspect->AddComponent<bool>( "enable", true );

    transform_aspect->AddImplementation( body_aspect->GetTransformAspectImpl() );

}

void MainLoopService::create_sphere( const Matrix& p_transform )
{
    RenderingAspect* rendering_aspect = m_sphereEntity.AddAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_sphereRender );

    rendering_aspect->AddComponent<MesheRenderingAspectImpl::PassSlot>( "sphere_texturepass_slot", "texture_pass" );

    
    RenderingNode* sphere_texturepass = rendering_aspect->GetComponent<MesheRenderingAspectImpl::PassSlot>( "sphere_texturepass_slot" )->getPurpose().GetRenderingNode();
    sphere_texturepass->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    sphere_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    sphere_texturepass->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    sphere_texturepass->GetFx()->GetShader( 0 )->LoadFromFile();
    sphere_texturepass->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet sphere_texturepass_rss;
    sphere_texturepass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    sphere_texturepass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    sphere_texturepass->GetFx()->SetRenderStates( sphere_texturepass_rss );


    sphere_texturepass->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    sphere_texturepass->GetMeshe()->SetImporter( m_meshe_import );
    sphere_texturepass->GetMeshe()->LoadFromFile( "planet.ac", 0 );

    sphere_texturepass->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "mars.jpg" ) ), 0 );
    sphere_texturepass->GetTexture( 0 )->LoadFromFile();

    m_sphereEntity.AddAspect<TransformAspect>();
    
    BodyAspect* body_aspect = m_sphereEntity.AddAspect<BodyAspect>();

    body_aspect->AddComponent<BodyAspect::SphereCollisionShape>( "shape", 2.0 );

    body_aspect->AddComponent<Matrix>( "attitude", p_transform );

    body_aspect->AddComponent<bool>( "collider", true );
    


    //body_aspect->AddComponent<bool>( "enable", true );

    TransformAspect* transform_aspect = m_sphereEntity.GetAspect<TransformAspect>();
    transform_aspect->AddImplementation( body_aspect->GetTransformAspectImpl() );
   
}