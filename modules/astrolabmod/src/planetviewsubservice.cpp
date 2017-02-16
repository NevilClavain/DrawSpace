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

#include <Windows.h>
#include "Psapi.h"
#include "planetviewsubservice.h"
#include "mainloopservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "planetviewsubservice", NULL )

#define LAYOUT_FILE "planetview.layout"

PlanetViewSubService::PlanetViewSubService( void ) :
m_planet_node( NULL ),
m_mouse_left( false ),
m_mouse_right( false ),
m_cdlodplanet_scenenodegraph( "cdlodplanet.SceneNodeGraph" ),
m_cdlodplanet_texturepass( "cdlodplanet.TexturePass" ),
m_camera_distance( 3000000.0 )
{
    m_guiwidgetpushbuttonclicked_cb = _DRAWSPACE_NEW_( GUIWidgetPushButtonClickedCallback, GUIWidgetPushButtonClickedCallback( this, &PlanetViewSubService::on_guipushbutton_clicked ) );
}
    
PlanetViewSubService::~PlanetViewSubService( void )
{
    _DRAWSPACE_DELETE_( m_guiwidgetpushbuttonclicked_cb );
}


void PlanetViewSubService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
}

void PlanetViewSubService::Init( DrawSpace::Logger::Configuration* p_logconf, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{
    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    _DSDEBUG( logger, dsstring("PlanetView sub service : startup...") );

    m_closeapp_cb = p_closeapp_cb;

    m_keysLinkTable.RegisterClientKey( &m_cdlodplanet_scenenodegraph );
    m_keysLinkTable.RegisterClientKey( &m_cdlodplanet_texturepass );

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );

    create_passes();
    create_camera();
    create_cubes();

    m_scenenodegraph.SetCurrentCamera( "camera" );

    m_cdlodplanet_scenenodegraph = &m_scenenodegraph;
    m_cdlodplanet_texturepass = m_texturepass;

    init_passes();

    m_renderer->GUI_LoadLayout( LAYOUT_FILE );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", 2 );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", 4 );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", 5 );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", 6 );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", 7 );
    
    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( LAYOUT_FILE, "Close_Button" );
}


void PlanetViewSubService::Run( void )
{
    m_scenenodegraph.ComputeTransformations( m_tm );

    m_texturepass->GetRenderingQueue()->Draw();
    m_finalpass->GetRenderingQueue()->Draw();

    char renderer_name[64];
    char fps[64];

    sprintf( fps, "%d fps", m_tm.GetFPS() );
    sprintf( renderer_name, "%s", m_pluginDescr.c_str() );

    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_FPS", fps );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_Renderer", renderer_name );


    char camera_distance_text[256];
    sprintf( camera_distance_text, "Camera distance : %.2f km", m_camera_distance / 1000.0 );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_CameraDistance", camera_distance_text );

    char working_set[64];

    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof( PROCESS_MEMORY_COUNTERS ) );

    sprintf( working_set, "%d bytes", pmc.WorkingSetSize );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_Mem", working_set );

    m_renderer->GUI_Render();

    m_renderer->FlipScreen();
    
    m_tm.Update();
    if( m_tm.IsReady() )
    {
    }
}

void PlanetViewSubService::Release( void )
{
    _DSDEBUG( logger, dsstring("PlanetView sub service : shutdown...") );
}

DrawSpace::Core::BaseSceneNode* PlanetViewSubService::InstanciateSceneNode( const dsstring& p_sceneNodeName )
{
    return NULL;
}

void PlanetViewSubService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetViewSubService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetViewSubService::ReleaseSceneNode( const dsstring& p_sceneNodeName )
{
}

void PlanetViewSubService::OnKeyPress( long p_key )
{
    m_renderer->GUI_OnKeyDown( p_key );
}

void PlanetViewSubService::OnEndKeyPress( long p_key )
{
    m_renderer->GUI_OnKeyUp( p_key );
}

void PlanetViewSubService::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:

            m_cdlodp_root->DumpMemoryAllocs();
            break;    
    }
}

void PlanetViewSubService::OnChar( long p_char, long p_scan )
{
    m_renderer->GUI_OnChar( p_char );
}

void PlanetViewSubService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( m_mouse_left )
    {
        m_objectRot->RotateAxis( Vector( 0.0, 1.0, 0.0, 1.0), p_dx * 8.0, m_tm );
        m_objectRot->RotateAxis( Vector( 1.0, 0.0, 0.0, 1.0), p_dy * 8.0, m_tm );
    }
    else if( m_mouse_right )
    {
        m_objectRot->RotateAxis( Vector( 0.0, 0.0, 1.0, 1.0), -p_dx * 8.0, m_tm );
    }

    m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void PlanetViewSubService::OnMouseWheel( long p_delta )
{
    if( p_delta > 0 )
    {
        m_camera_distance -= 100000.0;
    }
    else
    {
        m_camera_distance += 100000.0;
    }

    update_cameranodedistance();
}

void PlanetViewSubService::update_cameranodedistance( void )
{
    DrawSpace::Utils::Matrix camera_pos;
    camera_pos.Translation( 0.0, 0.0, m_camera_distance );

    m_camerapos->ClearAll();
    m_camerapos->PushMatrix( camera_pos );
}

void PlanetViewSubService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_mouse_left = true;
    m_renderer->GUI_OnMouseLeftButtonDown();
}

void PlanetViewSubService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_mouse_left = false;
    m_renderer->GUI_OnMouseLeftButtonUp();
}

void PlanetViewSubService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_mouse_right = true;
    m_renderer->GUI_OnMouseRightButtonDown();
}

void PlanetViewSubService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_mouse_right = false;
    m_renderer->GUI_OnMouseRightButtonUp();
}

void PlanetViewSubService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void PlanetViewSubService::on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id )
{
    if( p_layout != LAYOUT_FILE )
    {
        return;
    }

    if( "Close_Button" == p_widget_id )
    {
        MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETVIEW_CLOSEBUTTON_CLIC );
    }
}

void PlanetViewSubService::Activate( const dsstring& p_planetId )
{
    /// reset transformation

    m_camera_distance = 3000.0 * 1000.0;
    update_cameranodedistance();

    m_objectRot->Init( Vector( 0.0, 0.0, 0.0, 1.0 ) );

    ////


    m_renderer->GUI_SetLayout( LAYOUT_FILE );

    create_planet( p_planetId );
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
}

void PlanetViewSubService::Unactivate( void )
{
    destroy_planet();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
}

void PlanetViewSubService::create_passes( void )
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

void PlanetViewSubService::init_passes( void )
{
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
}

void PlanetViewSubService::create_camera( void )
{
    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );

    m_scenenodegraph.RegisterNode( m_camera_node );
   
    m_camerapos = _DRAWSPACE_NEW_( DrawSpace::Core::Transformation, DrawSpace::Core::Transformation );
    m_camerapos_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::Transformation>, SceneNode<DrawSpace::Core::Transformation>( "camera_pos" ) );

    m_camerapos_node->SetContent( m_camerapos );

    update_cameranodedistance();

    m_scenenodegraph.AddNode( m_camerapos_node );
    m_scenenodegraph.RegisterNode( m_camerapos_node );

    m_camera_node->LinkTo( m_camerapos_node );    
}

void PlanetViewSubService::create_planet( const dsstring& p_planetId )
{   
    m_planet_node = m_cdlodp_service->InstanciateSceneNode( p_planetId );

    m_scenenodegraph.RegisterNode( m_planet_node );

    m_planet_node->LinkTo( m_objectRot_node );
    m_cdlodp_service->RegisterScenegraphCallbacks( m_scenenodegraph );
}

void PlanetViewSubService::destroy_planet( void )
{
    m_cdlodp_service->UnregisterScenegraphCallbacks( m_scenenodegraph );

    m_scenenodegraph.UnregisterNode( m_planet_node );   
    m_planet_node->Unlink();

    m_planet_node = NULL;

    m_cdlodp_service->ReleaseSceneNode( "planet0" );
}

void PlanetViewSubService::create_cubes( void )
{
    m_cubescaling = _DRAWSPACE_NEW_( DrawSpace::Core::Transformation, DrawSpace::Core::Transformation );
    m_cubescaling_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::Transformation>, SceneNode<DrawSpace::Core::Transformation>( "cube_scaling" ) );

    m_cubescaling_node->SetContent( m_cubescaling );

    DrawSpace::Utils::Matrix cube_scale;
    cube_scale.Scale( Vector( 500000.0, 500000.0, 500000.0, 1.0 ) );

    m_cubescaling->PushMatrix( cube_scale );

    m_scenenodegraph.AddNode( m_cubescaling_node );
    m_scenenodegraph.RegisterNode( m_cubescaling_node );


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

void PlanetViewSubService::DumpMemoryAllocs( void )
{
    m_cdlodp_root->DumpMemoryAllocs();
}

void PlanetViewSubService::SetCDLODInfos( DrawSpace::Interface::Module::Root* p_cdlodp_root, DrawSpace::Interface::Module::Service* p_cdlodp_service )
{
    m_cdlodp_root = p_cdlodp_root;
    m_cdlodp_service = p_cdlodp_service;

    connect_keys( m_cdlodp_service );
}