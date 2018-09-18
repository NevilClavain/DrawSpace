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


#include "mainloopservice.h"
#include "drawspace.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "test03mainloopservice", NULL )

MainLoopService::MainLoopService( void ) :
m_left_mousebutton( false ),
m_right_mousebutton( false ),
m_current_camera( 0 ),
//m_worldsystem_evt_handler( this, &MainLoopService::on_transformsystem_evt ),
m_entitygraph_evt_cb( this, &MainLoopService::on_entitygraph_evt ),
m_camera_evt_cb( this, &MainLoopService::on_camera_evt ),
m_systems_update_evt_cb( this, &MainLoopService::on_systems_update_evt ),
//m_show_cube( true ),
m_cube_is_relative( false ),
m_attaching( 0 ),
m_detaching( 0 ),
m_fps_transformer( NULL ),
m_free_transformer( NULL)
//m_cube_is_relative( true )
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
    m_entitygraph.RegisterNodesEvtHandler( &m_entitygraph_evt_cb );

    (*p_mousecircularmode_cb)( true );
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
    rendering_aspect->AddComponent<TextRenderingAspectImpl::TextDisplay>( "datetime", 10, 50, 0, 255, 0, "..." );
    rendering_aspect->AddComponent<TextRenderingAspectImpl::TextDisplay>( "cubecontact_state", 10, 70, 0, 255, 0, "..." );


    m_rootEntityNode = m_entitygraph.SetRoot( &m_rootEntity );
    
    //////////////////////////////////////////////////////////////////////////

    TimeAspect* time_aspect = m_timeEntity.AddAspect<TimeAspect>();

    time_aspect->AddComponent<TimeManager>( "time_manager" );
    time_aspect->AddComponent<TimeAspect::TimeScale>( "time_scale", TimeAspect::NORMAL_TIME );
    time_aspect->AddComponent<dsstring>( "output_formated_datetime", "..." );

    time_aspect->AddComponent<dstime>( "time", 1576800000 + 12528056 );
    time_aspect->AddComponent<int>( "output_fps" );
    time_aspect->AddComponent<int>( "output_world_nbsteps" );

    time_aspect->AddComponent<dsreal>( "output_time_factor" );

    m_planet_rot = time_aspect->TimeAngleFactory( 0.0 );


    m_timeEntityNode = m_rootEntityNode.AddChild( &m_timeEntity );

    //////////////////////////////////////////////////////////////////////////

    time_aspect = m_camerasTimeEntity.AddAspect<TimeAspect>();

    time_aspect->AddComponent<TimeManager>( "time_manager" );
    time_aspect->AddComponent<TimeAspect::TimeScale>( "time_scale", TimeAspect::NORMAL_TIME );
    time_aspect->AddComponent<dsstring>( "output_formated_datetime", "..." );

    time_aspect->AddComponent<dstime>( "time", 0 );
    time_aspect->AddComponent<int>( "output_fps" );
    time_aspect->AddComponent<int>( "output_world_nbsteps" );

    time_aspect->AddComponent<dsreal>( "output_time_factor" );

    m_fps_yaw = time_aspect->TimeAngleFactory( 0.0 );
    m_fps_pitch = time_aspect->TimeAngleFactory( 0.0 );

    m_camerasTimeEntityNode = m_rootEntityNode.AddChild( &m_camerasTimeEntity );

    //////////////////////////////////////////////////////////////////////////

    PhysicsAspect* physic_aspect = m_world1Entity.AddAspect<PhysicsAspect>();

    //physic_aspect->AddComponent<bool>( "gravity_state", true );
    physic_aspect->AddComponent<bool>( "gravity_state", false );
    physic_aspect->AddComponent<Vector>( "gravity", Vector( 0.0, -9.81, 0.0, 0.0 ) );

    //m_World1EntityNode = m_rootEntityNode.AddChild( &m_world1Entity );

    m_World1EntityNode = m_timeEntityNode.AddChild( &m_world1Entity );

    //////////////////////////////////////////////////////////////////////////

    physic_aspect = m_world2Entity.AddAspect<PhysicsAspect>();

    //physic_aspect->AddComponent<bool>( "gravity_state", true );
    physic_aspect->AddComponent<bool>( "gravity_state", false );
    physic_aspect->AddComponent<Vector>( "gravity", Vector( 0.0, -9.81, 0.0, 0.0 ) );

    //m_World2EntityNode = m_rootEntityNode.AddChild( &m_world2Entity );

    m_World2EntityNode = m_timeEntityNode.AddChild( &m_world2Entity );

    //////////////////////////////////////////////////////////////////////////


    Matrix cube_transf;

    cube_transf.Translation( -20.0, 10.0, 20.0 );
    //cube_transf.Translation( 0.0, 0.0, -10.0 );
    create_cube( cube_transf, m_cubeEntity );

    create_ground();





    Matrix id;
    id.Identity();
    create_sphere( id, m_planet0Entity, &m_planet0Render, BodyAspect::ATTRACTOR_COLLIDER );

    create_sphere( id, m_planet1Entity, &m_planet1Render, BodyAspect::ATTRACTOR_COLLIDER );

    ///////////////////////////////////////////////////////////////////////////

    DrawSpace::Interface::Module::Root* mvtmod_root;

    if (!DrawSpace::Utils::PILoad::LoadModule("mvtmod", "mvt", &mvtmod_root))
    {
        _DSEXCEPTION("fail to load mvtmod module root")
    }

    m_fps_transformer = mvtmod_root->InstanciateTransformAspectImpls("fps");
    m_free_transformer = mvtmod_root->InstanciateTransformAspectImpls("free");


    ///////////////////////////////////////////////////////////////////////////

    TransformAspect* transform_aspect = m_cameraEntity.AddAspect<TransformAspect>();

    transform_aspect->SetImplementation( m_fps_transformer );
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

    transform_aspect->SetImplementation( m_free_transformer );
  
    transform_aspect->AddComponent<dsreal>( "rspeed_x", 0.0 );
    transform_aspect->AddComponent<dsreal>( "rspeed_y", 0.0 );
    transform_aspect->AddComponent<dsreal>( "rspeed_z", 0.0 );

    transform_aspect->AddComponent<Vector>( "speed" );
    transform_aspect->AddComponent<Vector>( "rot_axis_x", Vector( 1.0, 0.0, 0.0, 1.0 ) );
    transform_aspect->AddComponent<Vector>( "rot_axis_y", Vector( 0.0, 1.0, 0.0, 1.0 ) );
    transform_aspect->AddComponent<Vector>( "rot_axis_z", Vector( 0.0, 0.0, 1.0, 1.0 ) );

    transform_aspect->AddComponent<Matrix>( "pos" );
    transform_aspect->GetComponent<Matrix>( "pos" )->getPurpose().Translation( Vector( 0.0, 1.0, 2.0, 1.0 ) );

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




    //m_transformSystem.RegisterCameraEvtHandler( &m_worldsystem_evt_handler );
    //m_systemsHub.RegisterCameraEvtHandler( &m_worldsystem_evt_handler );

    m_systemsHub.RegisterSystemsUpdateEvtHandler( &m_systems_update_evt_cb );

    m_entitygraph.RegisterCameraEvtHandler( &m_camera_evt_cb );

    // ajouter la skybox a la scene
    m_skyboxEntityNode = m_rootEntityNode.AddChild( &m_skyboxEntity ); // comme la skybox n'a aucune interaction/influence avec le monde physique bullet, on peut la mettre directement sous rootEntity
                                                                        // mettre la skybox sous World1Entity fonctionne aussi, mais n'a aucune utilit�
    m_skyboxRender->RegisterToRendering( m_rendergraph );


    // ajouter le cube a la scene
    m_cubeEntityNode = m_World1EntityNode.AddChild( &m_cubeEntity );
    //m_cubeEntityNode = m_sphereEntityNode.AddChild( &m_cubeEntity );
    m_cubeRender.RegisterToRendering( m_rendergraph );

    
    // ajouter le ground a la scene
    m_groundEntityNode = m_World1EntityNode.AddChild( &m_groundEntity );
    m_groundRender.RegisterToRendering( m_rendergraph );


    ////////////////////////////////// planetes & orbites :)

    Matrix central_sphere_transf;
    central_sphere_transf.Translation( 0.0, 10.0, 10.0 );


    TransformAspect* center_planet0_transform = m_center_planet0_Entity.AddAspect<TransformAspect>();
    center_planet0_transform->SetImplementation( &m_center_planet0_transformer );
    center_planet0_transform->AddComponent<Matrix>( "attitude", central_sphere_transf );

    m_center_planet0_EntityNode = m_World2EntityNode.AddChild( &m_center_planet0_Entity );
    m_planet0EntityNode = m_center_planet0_EntityNode.AddChild( &m_planet0Entity );

    m_planet0Render.RegisterToRendering( m_rendergraph );




    TransformAspect* center_planet1_transform = m_center_planet1_Entity.AddAspect<TransformAspect>();
    center_planet1_transform->SetImplementation( &m_center_planet1_transformer );
    center_planet1_transform->AddComponent<dsreal>( "orbit_ray", 15.0 );
    center_planet1_transform->AddComponent<dsreal>( "excentricity", 0.99 );
    center_planet1_transform->AddComponent<dsreal>( "current_angle", 0.0 );
    center_planet1_transform->AddComponent<dsreal>( "orbit_duration", 1.0 );
    center_planet1_transform->AddComponent<dsreal>( "orbit_offset_rot", 0.0 );
    center_planet1_transform->AddComponent<dsreal>( "orbit_pan_angle", 0.0 );
    center_planet1_transform->AddComponent<dsreal>( "orbit_tilt_angle", 0.0 );
    center_planet1_transform->AddComponent<dsreal>( "orbit_translation_x", 0.0 );
    center_planet1_transform->AddComponent<dsreal>( "orbit_translation_z", 0.0 );
    center_planet1_transform->AddComponent<dsreal>( "axis_inclination", 67.0 );

    m_center_planet1_EntityNode = m_center_planet0_EntityNode.AddChild( &m_center_planet1_Entity );


    TransformAspect* revol_planet1_transform = m_revol_planet1_Entity.AddAspect<TransformAspect>();
    revol_planet1_transform->SetImplementation( &m_revol_planet1_transformer );

    revol_planet1_transform->AddComponent<dsreal>( "current_angle", 0.0 );
    revol_planet1_transform->AddComponent<dsreal>( "revol_duration", 1.0 );

    m_revol_planet1_EntityNode = m_center_planet1_EntityNode.AddChild( &m_revol_planet1_Entity );


    m_planet1EntityNode = m_revol_planet1_EntityNode.AddChild( &m_planet1Entity );
    m_planet1Render.RegisterToRendering( m_rendergraph );










    //////////////////////////////////////////////////////////////////////////////////////////

    // ajouter les cameras a la scene

    m_cameraEntityNode = m_camerasTimeEntityNode.AddChild( &m_cameraEntity );
    //m_camera2EntityNode = m_camerasTimeEntityNode.AddChild( &m_camera2Entity );

    m_camera2EntityNode = m_cubeEntityNode.AddChild( &m_camera2Entity );

    if( 0 == m_current_camera )
    {
        m_entitygraph.SetCurrentCameraEntity( &m_cameraEntity );
    }
    else
    {
        m_entitygraph.SetCurrentCameraEntity( &m_camera2Entity );
    }


    /////////////////////////////////////////////////////////////////////////////////

    m_systemsHub.Init( &m_entitygraph );

    m_rendergraph.PushSignal_UpdatedRenderingQueues();

    m_entitygraph.PushSignal_RenderSceneBegin();

    _DSDEBUG( logger, dsstring("main loop service : startup...") );
}

void MainLoopService::Run( void )
{
    m_systemsHub.Run( &m_entitygraph );
   
    TimeAspect* time_aspect = m_timeEntity.GetAspect<TimeAspect>();//m_rootEntity.GetAspect<TimeAspect>();
    char comment[256];
    //sprintf( comment, "%d fps - %s", time_aspect->GetFPS(), m_pluginDescr.c_str() );


    sprintf( comment, "%d fps - %s", time_aspect->GetComponent<int>( "output_fps" )->getPurpose(), m_pluginDescr.c_str() );

    RenderingAspect* rendering_aspect = m_rootEntity.GetAspect<RenderingAspect>();
    rendering_aspect->GetComponent<TextRenderingAspectImpl::TextDisplay>( "fps" )->getPurpose().m_text = comment;


    rendering_aspect->GetComponent<TextRenderingAspectImpl::TextDisplay>( "datetime" )->getPurpose().m_text = time_aspect->GetComponent<dsstring>( "output_formated_datetime" )->getPurpose();

    BodyAspect* cube_body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
    bool cube_contact = cube_body_aspect->GetComponent<bool>( "contact_state" )->getPurpose();

    rendering_aspect->GetComponent<TextRenderingAspectImpl::TextDisplay>( "cubecontact_state" )->getPurpose().m_text = cube_contact ? "contact !" : "...";


    m_planet_rot += 15.0;
    

    BodyAspect* body_aspect = m_planet0Entity.GetAspect<BodyAspect>();
    //TransformAspect* transform_aspect = m_center_planet0_Entity.GetAspect<TransformAspect>();

    Matrix planet_rot;
    planet_rot.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Utils::Maths::DegToRad( m_planet_rot.GetValue() ) );
    Matrix planet_mat = planet_rot;

    //transform_aspect->GetComponent<Matrix>( "attitude" )->getPurpose() = planet_mat;
    body_aspect->GetComponent<Matrix>( "attitude" )->getPurpose() = planet_mat;
    


        
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

    m_systemsHub.Release( &m_entitygraph );
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
                transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = 12.0; 
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
                transform_aspect->GetComponent<Vector>( "speed" )->getPurpose()[2] = -12.0;
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
            body_aspect->GetComponent<BodyAspect::Force>( "fwd_force" )->getPurpose().Enable();
        }
        break;

        case 'R':
        {
            BodyAspect* body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
            body_aspect->GetComponent<BodyAspect::Force>( "rev_force" )->getPurpose().Enable();
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
            body_aspect->GetComponent<BodyAspect::Force>( "fwd_force" )->getPurpose().Disable();
        }
        break;

        case 'R':
        {
            BodyAspect* body_aspect = m_cubeEntity.GetAspect<BodyAspect>();
            body_aspect->GetComponent<BodyAspect::Force>( "rev_force" )->getPurpose().Disable();
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
                    m_entitygraph.SetCurrentCameraEntity( &m_cameraEntity );
                }
                else
                {
                    m_entitygraph.SetCurrentCameraEntity( &m_camera2Entity );
                }
            }
            break;

        case VK_F3:
            {
                if( m_cube_is_relative )
                {
                    m_camera2EntityNode.Erase();
                    m_cubeEntityNode.Erase();

                    m_attaching = 1;
                    
                    /*
                    m_systemsHub.Run( &m_entitygraph );

                    m_cubeEntityNode = m_World1EntityNode.AddChild( &m_cubeEntity );
                    m_camera2EntityNode = m_cubeEntityNode.AddChild( &m_camera2Entity );

                    m_systemsHub.Run( &m_entitygraph );

                    m_cube_is_relative = false;
                    */
                }
                else
                {
                    m_camera2EntityNode.Erase();
                    m_cubeEntityNode.Erase();

                    m_detaching = 1;
                    
                    /*
                    m_systemsHub.Run( &m_entitygraph );
                    
                    m_cubeEntityNode = m_planet1EntityNode.AddChild( &m_cubeEntity );
                    m_camera2EntityNode = m_cubeEntityNode.AddChild( &m_camera2Entity );

                    m_systemsHub.Run( &m_entitygraph );

                    m_cube_is_relative = true;
                    */
                }

            }
            break;

        case VK_F4:
            {
                PhysicsAspect* physic_aspect = m_world1Entity.GetAspect<PhysicsAspect>();

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


        case VK_F5:
            {
                TimeAspect* time_aspect = m_timeEntity.GetAspect<TimeAspect>(); //m_rootEntity.GetAspect<TimeAspect>();
                time_aspect->GetComponent<TimeAspect::TimeScale>( "time_scale" )->getPurpose() = TimeAspect::NORMAL_TIME;        
            }
            break;

        case VK_F6:
            {
                TimeAspect* time_aspect = m_timeEntity.GetAspect<TimeAspect>();//m_rootEntity.GetAspect<TimeAspect>();
                time_aspect->GetComponent<TimeAspect::TimeScale>( "time_scale" )->getPurpose() = TimeAspect::SEC_1HOUR_TIME;        
            }
            break;


        case VK_F7:
            {
                TimeAspect* time_aspect = m_timeEntity.GetAspect<TimeAspect>();//m_rootEntity.GetAspect<TimeAspect>();
                time_aspect->GetComponent<TimeAspect::TimeScale>( "time_scale" )->getPurpose() = TimeAspect::SEC_30DAYS_TIME;        
            }
            break;

        case VK_F8:
            {
                TimeAspect* time_aspect = m_timeEntity.GetAspect<TimeAspect>();//m_rootEntity.GetAspect<TimeAspect>();
                time_aspect->GetComponent<TimeAspect::TimeScale>( "time_scale" )->getPurpose() = TimeAspect::FREEZE;        
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
            m_fps_yaw += - p_dx / 1.0;
            m_fps_pitch += - p_dy / 1.0;

            transform_aspect->GetComponent<dsreal>( "yaw" )->getPurpose() = m_fps_yaw.GetValue();
            transform_aspect->GetComponent<dsreal>( "pitch" )->getPurpose() = m_fps_pitch.GetValue();
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

/*
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
*/

void MainLoopService::on_camera_evt( DrawSpace::EntityGraph::EntityNodeGraph::CameraEvent p_evt, DrawSpace::Core::Entity* p_entity )
{
    if( EntityNodeGraph::CAMERA_ACTIVE == p_evt && p_entity )
    {
        CameraAspect* curr_camera_aspect = p_entity->GetAspect<CameraAspect>();
        RenderingAspect* rendering_aspect = m_rootEntity.GetAspect<RenderingAspect>();
    
        // mise a jour affichage avec le nom de la camera courante...
        rendering_aspect->GetComponent<dsstring>( "current camera" )->getPurpose() = curr_camera_aspect->GetComponent<dsstring>( "camera_debug_name" )->getPurpose();
    }
}

void MainLoopService::on_systems_update_evt( DrawSpace::Systems::Hub::SystemsUpdateEvent p_evt )
{
    if( DrawSpace::Systems::Hub::SYSTEMS_UPDATE_END == p_evt )
    {
        if( 1 == m_attaching )
        {
            m_cubeEntityNode = m_World1EntityNode.AddChild( &m_cubeEntity );
            m_camera2EntityNode = m_cubeEntityNode.AddChild( &m_camera2Entity );

            m_attaching = 2;
        }
        else if( 2 == m_attaching )
        {
            m_cube_is_relative = false;
            m_attaching = 0;
        }
        else if( 1 == m_detaching )
        {
            m_cubeEntityNode = m_planet1EntityNode.AddChild( &m_cubeEntity );
            m_camera2EntityNode = m_cubeEntityNode.AddChild( &m_camera2Entity );

            m_detaching = 2;
        }
        else if( 2 == m_detaching )
        {
            m_cube_is_relative = true;
            m_detaching = 0;
        }
    }
}



void MainLoopService::on_entitygraph_evt( DrawSpace::EntityGraph::EntityNode::Event p_evt, DrawSpace::Core::Entity* p_entity )
{

}

void MainLoopService::create_cube( const Matrix& p_transform, DrawSpace::Core::Entity& p_entity )
{
    RenderingAspect* rendering_aspect = p_entity.AddAspect<RenderingAspect>();

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

    p_entity.AddAspect<TransformAspect>();
    
    BodyAspect* body_aspect = p_entity.AddAspect<BodyAspect>();

    body_aspect->AddComponent<BodyAspect::BoxCollisionShape>( "shape", Vector( 0.5, 0.5, 0.5, 1.0 ) );

    body_aspect->AddComponent<Matrix>( "attitude", p_transform );

    body_aspect->AddComponent<dsreal>( "mass", 7.0 );

    body_aspect->AddComponent<BodyAspect::Force>( "fwd_force", Vector( 0.0, 0.0, 90.0, 1.0 ) );
    body_aspect->AddComponent<BodyAspect::Force>( "rev_force", Vector( 0.0, 0.0, -90.0, 1.0 ) );

    body_aspect->AddComponent<BodyAspect::Torque>( "torque", Vector( 0.0, 7.0, 0.0, 1.0 ) );
    body_aspect->AddComponent<BodyAspect::Torque>( "torque_neg", Vector( 0.0, -7.0, 0.0, 1.0 ) );

    body_aspect->AddComponent<BodyAspect::Mode>( "mode", BodyAspect::BODY );

    body_aspect->AddComponent<bool>( "enable", true );
    body_aspect->AddComponent<bool>( "contact_state", false );

    TransformAspect* transform_aspect = p_entity.GetAspect<TransformAspect>();
    transform_aspect->SetImplementation( body_aspect->GetTransformAspectImpl() );
    
}

void MainLoopService::create_skybox( void )
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

    rendering_aspect->AddComponent<std::vector<dsstring>>( "skybox_passes", skybox_passes );

    ////////////// 6 jeux de 32 textures stages

    /*
    std::array<std::array<Texture*,RenderingNode::NbMaxTextures>,6> skybox_textures = { NULL };

    skybox_textures[0][0] = _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) );
    skybox_textures[1][0] = _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) );
    skybox_textures[2][0] = _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) );
    skybox_textures[3][0] = _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) );
    skybox_textures[4][0] = _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) );
    skybox_textures[5][0] = _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) );

    for( int i = 0; i < 6; i++ )
    {
        skybox_textures[i][0]->LoadFromFile();    
    }

    rendering_aspect->AddComponent<std::array<std::array<Texture*,RenderingNode::NbMaxTextures>,6>>( "skybox_textures", skybox_textures );
    rendering_aspect->AddComponent<std::array<std::array<Texture*,RenderingNode::NbMaxTextures>,6>>( "skybox_mirror_textures", skybox_textures );
    */

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
        textures[i][0]->LoadFromFile();

        skybox_textures.push_back(textures[i]);
    }
    rendering_aspect->AddComponent<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>("skybox_textures", skybox_textures);
    rendering_aspect->AddComponent<std::vector<std::array<Texture*, RenderingNode::NbMaxTextures>>>("skybox_mirror_textures", skybox_textures);


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


    //////////////// valeur du rendering order pour chaque slot pass
    rendering_aspect->AddComponent<int>( "skybox_ro", -1000 );


    TransformAspect* transform_aspect = m_skyboxEntity.AddAspect<TransformAspect>();

    transform_aspect->SetImplementation( &m_transformer );

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
    
    //ground_attitude.Identity();
    ground_attitude.Translation( 0.0, -100.0, 0.0 );

    body_aspect->AddComponent<Matrix>( "attitude", ground_attitude );

    
    body_aspect->AddComponent<BodyAspect::Mode>( "mode", BodyAspect::COLLIDER );

    body_aspect->AddComponent<bool>( "enable", true );
    body_aspect->AddComponent<bool>( "contact_state", false );

    transform_aspect->SetImplementation( body_aspect->GetTransformAspectImpl() );

}

void MainLoopService::create_sphere( const Matrix& p_transform, DrawSpace::Core::Entity& p_entity, DrawSpace::AspectImplementations::MesheRenderingAspectImpl* p_render, BodyAspect::Mode p_mode )
{
    RenderingAspect* rendering_aspect = p_entity.AddAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( p_render );

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

    p_entity.AddAspect<TransformAspect>();
    
    BodyAspect* body_aspect = p_entity.AddAspect<BodyAspect>();

    body_aspect->AddComponent<BodyAspect::SphereCollisionShape>( "shape", 2.0 );

    body_aspect->AddComponent<Matrix>( "attitude", p_transform );

    body_aspect->AddComponent<BodyAspect::Mode>( "mode", p_mode );

    body_aspect->AddComponent<bool>( "enable", true );
    body_aspect->AddComponent<bool>( "contact_state", false );

    TransformAspect* transform_aspect = p_entity.GetAspect<TransformAspect>();
    transform_aspect->SetImplementation( body_aspect->GetTransformAspectImpl() );
   
}
