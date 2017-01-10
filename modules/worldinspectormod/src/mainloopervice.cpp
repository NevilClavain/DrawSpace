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


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "worldinspectormainloopservice", NULL )

MainLoopService::MainLoopService( void ) :
m_mouse_left( false ),
m_mouse_right( false ),
m_cdlodplanet_scenenodegraph( "cdlodplanet.SceneNodeGraph" ),
m_cdlodplanet_texturepass( "cdlodplanet.TexturePass" )
{
    m_guiwidgetpushbuttonclicked_cb = _DRAWSPACE_NEW_( GUIWidgetPushButtonClickedCallback, GUIWidgetPushButtonClickedCallback( this, &MainLoopService::on_guipushbutton_clicked ) );
    
}

MainLoopService::~MainLoopService( void )
{
    _DRAWSPACE_DELETE_( m_guiwidgetpushbuttonclicked_cb );
}

void MainLoopService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{

}

void MainLoopService::Init( DrawSpace::Logger::Configuration* p_logconf, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                            DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{

    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    _DSDEBUG( logger, dsstring("main loop service : startup...") );

   
    // hide OS mouse cursor (we use CEGUI mouse cursor instead)
    (*p_mousevisible_cb)( false );

    m_closeapp_cb = p_closeapp_cb;

    
    m_keysLinkTable.RegisterClientKey( &m_cdlodplanet_scenenodegraph );
    m_keysLinkTable.RegisterClientKey( &m_cdlodplanet_texturepass );
    


    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );
    
    //create_passes();
    
    //create_camera();
    //create_cubes();

    //m_scenenodegraph.SetCurrentCamera( "camera" );

    //load_cdlodplanet_module();

    //create_planet();
    
    //init_passes();
    
    m_renderer->GUI_InitSubSystem();

    m_renderer->GUI_SetResourcesRootDirectory( "./xfskin2" );
    m_renderer->GUI_LoadScheme( "xfskin.scheme" );

    m_renderer->GUI_SetMouseCursorImage( "xfskin/MouseCursor" );
    m_renderer->GUI_ShowMouseCursor( true );

    
    m_renderer->GUI_LoadLayout( "worldinspector.layout" );
    m_renderer->GUI_SetLayout( "worldinspector.layout" );

    /*
    m_renderer->GUI_StoreWidget( "worldinspector.layout", "root", 1 );
    m_renderer->GUI_StoreWidget( "worldinspector.layout", "root", 2 );
    
    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( "worldinspector.layout", "Close_Button" );
    */


    PlanetSetupSubService::GetInstance()->Init( p_logconf, p_mousecircularmode_cb, p_mousevisible_cb, p_closeapp_cb );

    m_current_subservice = PlanetSetupSubService::GetInstance();
}

void MainLoopService::Run( void )
{   
    //m_scenenodegraph.ComputeTransformations( m_tm );

    //m_texturepass->GetRenderingQueue()->Draw();
    //m_finalpass->GetRenderingQueue()->Draw();

    //m_renderer->DrawText( 255, 0, 0, 10, 20, "%d fps -- %s", m_tm.GetFPS(), m_pluginDescr.c_str() );

    /*
    char comment[256];
    sprintf( comment, "%d fps - %s", m_tm.GetFPS(), m_pluginDescr.c_str() );
    dsstring fps_text = comment;
    m_renderer->GUI_SetWidgetText( "worldinspector.layout", "FPS_Label", fps_text );
    

    m_renderer->GUI_Render();
    

    m_renderer->FlipScreen();

    */

    /*
    m_renderer->BeginScreen();

    m_renderer->ClearScreen( 0, 0, 0, 0 );

    m_renderer->GUI_Render();

    m_renderer->EndScreen();

    m_renderer->FlipScreen();
    
    m_tm.Update();
    if( m_tm.IsReady() )
    {
    }
    */

    m_current_subservice->Run();
}

void MainLoopService::Release( void )
{
    _DSDEBUG( logger, dsstring("main loop service : shutdown...") );
}

DrawSpace::Core::BaseSceneNode* MainLoopService::InstanciateSceneNode( const dsstring& p_sceneNodeName )
{
    return NULL;
}

void MainLoopService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void MainLoopService::ReleaseSceneNode( const dsstring& p_sceneNodeName )
{
}

void MainLoopService::OnKeyPress( long p_key )
{
    //m_renderer->GUI_OnKeyDown( p_key );

    m_current_subservice->OnKeyPress( p_key );
}

void MainLoopService::OnEndKeyPress( long p_key )
{
    //m_renderer->GUI_OnKeyUp( p_key );

    m_current_subservice->OnEndKeyPress( p_key );
}

void MainLoopService::OnKeyPulse( long p_key )
{
}

void MainLoopService::OnChar( long p_char, long p_scan )
{
    //m_renderer->GUI_OnChar( p_char );

    m_current_subservice->OnChar( p_char, p_scan );
}

void MainLoopService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    /*
    if( m_mouse_left )
    {
        m_objectRot->RotateAxis( Vector( 0.0, 1.0, 0.0, 1.0), p_dx * 8.0, m_tm );
        m_objectRot->RotateAxis( Vector( 1.0, 0.0, 0.0, 1.0), p_dy * 8.0, m_tm );
    }
    else if( m_mouse_right )
    {
        m_objectRot->RotateAxis( Vector( 0.0, 0.0, 1.0, 1.0), -p_dx * 8.0, m_tm );
    }
    */

    //m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );

    m_current_subservice->OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void MainLoopService::OnMouseWheel( long p_delta )
{
}

void MainLoopService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_mouse_left = true;

    //m_renderer->GUI_OnMouseLeftButtonDown();

    m_current_subservice->OnMouseLeftButtonDown( p_xm, p_ym );
}

void MainLoopService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_mouse_left = false;

    //m_renderer->GUI_OnMouseLeftButtonUp();

    m_current_subservice->OnMouseLeftButtonUp( p_xm, p_ym );
}

void MainLoopService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_mouse_right = true;

    //m_renderer->GUI_OnMouseRightButtonDown();

    m_current_subservice->OnMouseRightButtonDown( p_xm, p_ym );
}

void MainLoopService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_mouse_right = false;

    //m_renderer->GUI_OnMouseRightButtonUp();

    m_current_subservice->OnMouseRightButtonUp( p_xm, p_ym );
}

void MainLoopService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void MainLoopService::init_passes( void )
{
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
}

void MainLoopService::create_passes( void )
{
    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->Initialize();
    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 0 );


    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();

    m_finalpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_finalpass->GetRenderingQueue()->SetTargetClearingColor( 255, 255, 255, 255 );

    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    

    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "point" ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );


    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );

}

void MainLoopService::create_cubes( void )
{
    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_chunk->RegisterPassSlot( m_texturepass );


    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    
    m_chunk->GetMeshe()->SetImporter( m_meshe_import );
    m_chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
  
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "none" ) );

    

    m_chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "chunk" ) );
    m_chunk_node->SetContent( m_chunk );

    
    m_scenenodegraph.RegisterNode( m_chunk_node );


    m_objectRot = _DRAWSPACE_NEW_( DrawSpace::Core::FreeMovement, DrawSpace::Core::FreeMovement );
    m_objectRot_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FreeMovement>, SceneNode<DrawSpace::Core::FreeMovement>( "objectRot" ) );

    m_objectRot->Init( Vector( 0.0, 0.0, 0.0, 1.0 ) );
    m_objectRot->SetSpeed( 0.0 );

    m_objectRot_node->SetContent( m_objectRot );

    m_scenenodegraph.AddNode( m_objectRot_node );
    m_scenenodegraph.RegisterNode( m_objectRot_node );

    m_chunk_node->LinkTo( m_objectRot_node );
}

void MainLoopService::create_camera( void )
{
    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );

    m_scenenodegraph.RegisterNode( m_camera_node );

    

    m_camerapos = _DRAWSPACE_NEW_( DrawSpace::Core::Transformation, DrawSpace::Core::Transformation );
    m_camerapos_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::Transformation>, SceneNode<DrawSpace::Core::Transformation>( "camera_pos" ) );

    m_camerapos_node->SetContent( m_camerapos );

    DrawSpace::Utils::Matrix camera_pos;
    camera_pos.Translation( 0.0, 0.0, 3000.0 * 1000.0 );

    m_camerapos->PushMatrix( camera_pos );

    m_scenenodegraph.AddNode( m_camerapos_node );
    m_scenenodegraph.RegisterNode( m_camerapos_node );

    m_camera_node->LinkTo( m_camerapos_node );    
}

void MainLoopService::create_planet( void )
{   
    DrawSpace::Core::BaseSceneNode* spacebox_node = m_cdlodp_service->InstanciateSceneNode( "planet0" );

    m_scenenodegraph.RegisterNode( spacebox_node );

    spacebox_node->LinkTo( m_objectRot_node );
    m_cdlodp_service->RegisterScenegraphCallbacks( m_scenenodegraph );
}

void MainLoopService::load_cdlodplanet_module( void )
{
    if( !DrawSpace::Utils::PILoad::LoadModule( "cdlodplanetmod", "cdlodplanet", &m_cdlodp_root ) )
    {
        _DSEXCEPTION( "fail to load cdlodplanet module root" )
    }    
    m_cdlodp_service = m_cdlodp_root->InstanciateService( "cdlodplanet" );
    connect_keys( m_cdlodp_service );

    m_cdlodplanet_scenenodegraph = &m_scenenodegraph;
    m_cdlodplanet_texturepass = m_texturepass;

    m_cdlodp_service->Init( DrawSpace::Logger::Configuration::GetInstance(), NULL, NULL, NULL );
}

void MainLoopService::on_guipushbutton_clicked( dsstring p_widget_id )
{
    if( "Close_Button" == p_widget_id )
    {
        (*m_closeapp_cb)( 0 );
    }
}

void MainLoopService::SetPlanetViewLayout( void )
{

}

void MainLoopService::SetPlanetSetupLayout( void )
{

}