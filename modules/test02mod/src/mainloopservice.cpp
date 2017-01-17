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
#include "test02root.h"
#include "drawspace.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "test02mainloopservice", NULL )

MainLoopService::MainLoopService( void ) : 
m_fpsmove( true ),
m_waves( 0.0 ),
m_waves_inc( true ),
m_skybox_texturepass( "skybox.TexturePass" ),
m_skybox_texturemirrorpass( "skybox.TextureMirrorPass" ),
m_skybox_reflectornormale( "skybox.ReflectorNormale" ),
m_skybox_texturesbankpath( "skybox.TexturesBankPath" ),
m_skybox_texturesbankvirtualfspath( "skybox.TexturesBankVirtualFSPath" ),
m_skybox_texturesnames( "skybox.TexturesNames" ),
m_hmi_mode( true )
{
    m_guiwidgetpushbuttonclicked_cb = _DRAWSPACE_NEW_( GUIWidgetPushButtonClickedCallback, GUIWidgetPushButtonClickedCallback( this, &MainLoopService::on_guipushbutton_clicked ) );
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
    m_keysLinkTable.RegisterClientKey( &m_skybox_texturepass );
    m_keysLinkTable.RegisterClientKey( &m_skybox_texturemirrorpass );
    m_keysLinkTable.RegisterClientKey( &m_skybox_reflectornormale );
    m_keysLinkTable.RegisterClientKey( &m_skybox_texturesbankpath );
    m_keysLinkTable.RegisterClientKey( &m_skybox_texturesbankvirtualfspath );
    m_keysLinkTable.RegisterClientKey( &m_skybox_texturesnames );


    //////////////////////////////////////////////////////////////////////////////

    m_mousecircularmode_cb = p_mousecircularmode_cb;
    m_closeapp_cb = p_closeapp_cb;

    // hide OS mouse cursor (we use CEGUI mouse cursor instead)
    //(*p_mousevisible_cb)( false );

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

    m_world.Initialize();
    m_world.SetGravity( DrawSpace::Utils::Vector( 0.0, -9.81, 0.0, 0.0 ) );

    load_skybox_module();

    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    
    create_passes();
    create_spacebox();
    create_camera();
    create_static_cube();
    create_ground();

    init_passes();
    
    m_scenenodegraph.SetCurrentCamera( "camera" );

    m_renderer->GUI_InitSubSystem();
    m_renderer->GUI_SetResourcesRootDirectory( "./testskin" );
    
    m_renderer->GUI_LoadScheme( "AlfiskoSkin.scheme" );

    m_renderer->GUI_LoadLayout( "main.layout" );

    m_renderer->GUI_SetLayout( "main.layout" );

    /*
    m_renderer->GUI_StoreWidget( 0, 1 );
    m_renderer->GUI_StoreWidget( 1, 2 );
    m_renderer->GUI_StoreWidget( 1, 3 );
    m_renderer->GUI_StoreWidget( 1, 4 );
    */

    m_renderer->GUI_StoreWidget( "main.layout", "root", 1 );
    m_renderer->GUI_StoreWidget( "main.layout", "testWindow", 2 );
    m_renderer->GUI_StoreWidget( "main.layout", "testWindow", 3 );
    m_renderer->GUI_StoreWidget( "main.layout", "testWindow", 4 );
    m_renderer->GUI_StoreWidget( "main.layout", "root", 5 );
    m_renderer->GUI_StoreWidget( "main.layout", "root", 6 );
    


    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( "main.layout", "Trigger" );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( "main.layout", "Quit" );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( "main.layout", "Button_Create" );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( "main.layout", "Button_Destroy" );


    set_mouse_circular_mode( false );

    
    
    _DSDEBUG( logger, dsstring( "main loop service : startup..." ) );
}

void MainLoopService::Run( void )
{   

    m_scenenodegraph.ComputeTransformations( m_tm );


    Matrix water_plane_mat;
    m_ground_body_node->GetFinalTransform( water_plane_mat );


    Vector reflectorPos( water_plane_mat( 3, 0 ), water_plane_mat( 3, 1 ), water_plane_mat( 3, 2 ), 1.0 );
    Vector reflectorNormale( 0.0, 1.0, 0.0, 1.0 );


    /*
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_pos", reflectorPos );
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_normale", reflectorNormale );
    */

    for( size_t i = 0; i < m_cubes.size(); i++ )
    {
        m_cubes[i].chunk->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_pos", reflectorPos );
        m_cubes[i].chunk->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_normale", reflectorNormale );
    }

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_pos", reflectorPos );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_normale", reflectorNormale );

    m_skybox_reflectornormale = reflectorNormale;

    
    m_sb_service->Run();

    m_coloredpass->GetRenderingQueue()->Draw();

    m_wavespass->GetRenderingQueue()->Draw();
    m_texturepass->GetRenderingQueue()->Draw();
    m_texturemirrorpass->GetRenderingQueue()->Draw();
    m_bumppass->GetRenderingQueue()->Draw();
    m_finalpass->GetRenderingQueue()->Draw();

    char comment[256];
    sprintf( comment, "%d fps - %s", m_tm.GetFPS(), m_pluginDescr.c_str() );
    dsstring fps_text = comment;

    //m_renderer->DrawText( 255, 0, 0, 10, 20, "%s", fps_text.c_str() );   
    m_renderer->GUI_SetWidgetText( "main.layout", "FPSInfoLabel", fps_text );


    m_renderer->GUI_Render();

    m_renderer->FlipScreen();

    m_tm.Update();
    if( m_tm.IsReady() )
    {
        m_world.StepSimulation( m_tm.GetFPS(), 15 );

        if( m_waves_inc )
        {
            if( m_waves < 200.0 )
            {
                m_tm.TranslationSpeedInc( &m_waves, 1.0 );
            }
            else
            {
                m_waves_inc = false;
            }
        }
        else
        {
            if( m_waves > 0.0 )
            {
                m_tm.TranslationSpeedDec( &m_waves, 1.0 );
            }
            else
            {
                m_waves_inc = true;
            }        
        }
    }

    m_wavespass->GetViewportQuad()->SetShaderRealVector( "waves", DrawSpace::Utils::Vector( m_waves, 0.0, 0.0, 0.0 ) );
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

void MainLoopService::set_mouse_circular_mode( bool p_state )
{
    if( m_mousecircularmode_cb )
    {
        (*m_mousecircularmode_cb)( p_state );

        m_renderer->GUI_ShowMouseCursor( !p_state );
    }
}

void MainLoopService::OnKeyPress( long p_key )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnKeyDown( p_key );
        return;
    }

    switch( p_key )
    {
        case 'Q':

            m_fpsmove.SetSpeed( 20.0 );
            break;

        case 'W':

            m_fpsmove.SetSpeed( -20.0 );
            break;

        case VK_F3:

            //m_cube_body->Enable( true );
            break;

    }
}

void MainLoopService::OnEndKeyPress( long p_key )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnKeyUp( p_key );
        return;
    }

    switch( p_key )
    {
        case 'Q':
        case 'W':

            m_fpsmove.SetSpeed( 0.0 );
            break;      
    }
}

void MainLoopService::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_SPACE:
            {
                // read render texture content test...
                m_coloredpass->GetTargetTexture()->CopyTextureContent();

                unsigned char* pix = (unsigned char*)m_texturecontent;

                unsigned char b = pix[0];
                unsigned char g = pix[1];
                unsigned char r = pix[2];
                unsigned char a = pix[3];
            }
            break;

        case VK_F1:

            m_hmi_mode = !m_hmi_mode;
            set_mouse_circular_mode( !m_hmi_mode );
            
            break;
    }
}

void MainLoopService::OnChar( long p_char, long p_scan )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnChar( p_char );
        return;
    }
}

void MainLoopService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( !m_hmi_mode )
    {
        m_fpsmove.RotateYaw( - p_dx / 4.0, m_tm );
	    m_fpsmove.RotatePitch( - p_dy / 4.0, m_tm );
    }
    else
    {
        m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
    }
}

void MainLoopService::OnMouseWheel( long p_delta )
{
}

void MainLoopService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnMouseLeftButtonDown();
    }
}

void MainLoopService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnMouseLeftButtonUp();
    }
}

void MainLoopService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnMouseRightButtonDown();
    }
}

void MainLoopService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    if( m_hmi_mode )
    {
        m_renderer->GUI_OnMouseRightButtonUp();
    }
}

void MainLoopService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}



void MainLoopService::create_passes( void )
{
    /////////////////////////////////////////////////////////////////////////////////////////////////

    m_coloredpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "colored_pass" ) );
    m_coloredpass->SetRenderTarget( Texture::RENDERTARGET_CPU );
    m_coloredpass->Initialize();
    m_coloredpass->GetRenderingQueue()->EnableDepthClearing( true );
    m_coloredpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_coloredpass->GetRenderingQueue()->SetTargetClearingColor( 12, 66, 155, 255 );


    /////////////////////////////////////////////////////////////////////////////////////////////////


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );
    m_texturepass->Initialize();
    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );


    m_texturemirrorpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texturemirror_pass" ) );

    m_texturemirrorpass->Initialize();
    m_texturemirrorpass->GetRenderingQueue()->EnableDepthClearing( true );



    m_bumppass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "bump_pass" ) );

    m_bumppass->SetRenderPurpose( Texture::RENDERPURPOSE_FLOATVECTOR );

    m_bumppass->Initialize();
    
    m_bumppass->GetRenderingQueue()->EnableDepthClearing( true );
    m_bumppass->GetRenderingQueue()->EnableTargetClearing( true );
    m_bumppass->GetRenderingQueue()->SetTargetClearingColor( 255, 255, 255, 255 );




    m_wavespass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "waves_pass" ) );

    m_wavespass->SetTargetDimsFromRenderer( false );
    m_wavespass->SetTargetDims( 512, 512 );

    m_wavespass->Initialize();
    m_wavespass->CreateViewportQuad();
    
    m_wavespass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_wavespass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_waves.vso", true ) ) );
    m_wavespass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_waves.pso", true ) ) );
    m_wavespass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_wavespass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    
    m_wavespass->GetViewportQuad()->AddShaderParameter( 1, "waves", 0 );




    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();

    m_finalpass->GetRenderingQueue()->SetTargetClearingColor( 255, 255, 255, 255 );

    m_finalpass->CreateViewportQuad( /*-2.0*/ );
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    

    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "point" ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    //m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    //m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_mask.vso", true ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_mask.pso", true ) ) );

    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    m_finalpass->GetViewportQuad()->SetTexture( m_texturemirrorpass->GetTargetTexture(), 1 );
    m_finalpass->GetViewportQuad()->SetTexture( m_bumppass->GetTargetTexture(), 2 );

    // puisque m_coloredpass n'est connect�e a aucune passe
    void* data;
    m_renderer->CreateTexture( m_coloredpass->GetTargetTexture(), &data );
  

    m_skybox_texturepass = m_texturepass;
    m_skybox_texturemirrorpass = m_texturemirrorpass;
}

void MainLoopService::init_passes( void )
{
    m_coloredpass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturemirrorpass->GetRenderingQueue()->UpdateOutputQueue();
    m_bumppass->GetRenderingQueue()->UpdateOutputQueue();
    m_wavespass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();

    m_coloredpass->GetTargetTexture()->AllocTextureContent();
    m_texturecontent = m_coloredpass->GetTargetTexture()->GetTextureContentPtr();

    m_texcube2->AllocTextureContent();
    m_tc2content = m_texcube2->GetTextureContentPtr();

    unsigned char* tptr = (unsigned char*)m_tc2content;

    for( int y = 0; y < 256; y++ )
    {
        for( int x = 0; x < 256; x++ )
        {
            int index = 4 * ( ( 256 * y ) + x );

            tptr[index] = 0;
            tptr[index + 1] = 0;
            tptr[index + 2] = 255;
            tptr[index + 3] = 255;
        }
    }

    m_texcube2->UpdateTextureContent();

}

void MainLoopService::load_skybox_module( void )
{
    if( !DrawSpace::Utils::PILoad::LoadModule( "skyboxmod", "skybox", &m_sbmod_root ) )
    {
        _DSEXCEPTION( "fail to load skyboxmod module root" )
    }

    m_sb_service = m_sbmod_root->InstanciateService( "skybox" );
    if( NULL == m_sb_service )
    {
        _DSEXCEPTION( "fail to load skybox module service" )
    }
    connect_keys( m_sb_service );

    //m_skybox_scenenodegraph = &m_scenenodegraph;
    m_skybox_texturesbankpath = "test_data/textures_bank";
    m_skybox_texturesbankvirtualfspath = "test_data.bank";

    std::vector<dsstring> textures_names;
    textures_names.assign( 6, "" );
    textures_names[0] = "sb0.bmp";
    textures_names[1] = "sb2.bmp";
    textures_names[2] = "sb3.bmp";
    textures_names[3] = "sb1.bmp";
    textures_names[4] = "sb4.bmp";
    textures_names[5] = "sb4.bmp";
    m_skybox_texturesnames = textures_names;
    
    m_sb_service->Init( DrawSpace::Logger::Configuration::GetInstance(), NULL, NULL, NULL );
}


void MainLoopService::create_spacebox( void )
{ 
    DrawSpace::Core::BaseSceneNode* spacebox_node = m_sb_service->InstanciateSceneNode( "spacebox" );

    m_scenenodegraph.RegisterNode( spacebox_node );

    m_spacebox_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "spacebox_transfo" ) );
    m_spacebox_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix spacebox_scale;
    spacebox_scale.Scale( 20.0, 20.0, 20.0 );
    m_spacebox_transfo_node->GetContent()->PushMatrix( spacebox_scale );
    
    m_scenenodegraph.AddNode( m_spacebox_transfo_node );
    m_scenenodegraph.RegisterNode( m_spacebox_transfo_node );

    spacebox_node->LinkTo( m_spacebox_transfo_node );
}


void MainLoopService::create_camera( void )
{
    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );

    m_scenenodegraph.RegisterNode( m_camera_node );

    m_scenenodegraph.SetCurrentCamera( "camera" );
    

    m_fpsmove.Init( DrawSpace::Utils::Vector( 0.0, 1.0, 10.0, 1.0 ) );
    m_fpsmove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FPSMovement>, SceneNode<DrawSpace::Core::FPSMovement>( "fps_node" ) );
    m_fpsmove_node->SetContent( &m_fpsmove );
    

    m_scenenodegraph.AddNode( m_fpsmove_node );
    m_scenenodegraph.RegisterNode( m_fpsmove_node );
    
    m_camera_node->LinkTo( m_fpsmove_node );
}

void MainLoopService::create_dynamic_cube( void )
{
    CUBE_ENTRY ce;

    ce.chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    ce.chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    ce.chunk->RegisterPassSlot( m_texturepass );
    ce.chunk->RegisterPassSlot( m_texturemirrorpass );
    
    ce.chunk->GetMeshe()->SetImporter( m_meshe_import );
    ce.chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );

    ce.chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
  
    ce.chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    ce.chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    ce.chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    ce.chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    ce.chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    ce.chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    ce.chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    ce.chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );


    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );

    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->GetTexture( 0 )->LoadFromFile();

    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "reflector_pos", 24 );
    ce.chunk->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "reflector_normale", 25 );
    
    char num[24];
    sprintf( num, "chunk_cube_%d", m_cubes.size() );
    dsstring chunk_id = num;
    
    ce.chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( chunk_id ) );
    ce.chunk_node->SetContent( ce.chunk );

    m_scenenodegraph.RegisterNode( ce.chunk_node );

    DrawSpace::Dynamics::Body::Parameters cube_params;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;

    cube_params.initial_attitude.Translation( 0.0, 3.5, 0.0 );


    ce.cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, cube_params ) );

    ce.cube_body->Enable( true );


    sprintf( num, "dynamic_cube_%d", m_cubes.size() );
    dsstring cube_id = num;

    ce.cube_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( cube_id ) );
    ce.cube_body_node->SetContent( ce.cube_body );

    m_scenenodegraph.AddNode( ce.cube_body_node );
    m_scenenodegraph.RegisterNode( ce.cube_body_node );

    ce.chunk_node->LinkTo( ce.cube_body_node );

    m_cubes.push_back( ce );

    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturemirrorpass->GetRenderingQueue()->UpdateOutputQueue();
}

void MainLoopService::create_static_cube( void )
{


    /*
    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_chunk->RegisterPassSlot( m_texturepass );
    m_chunk->RegisterPassSlot( m_texturemirrorpass );
    
    m_chunk->GetMeshe()->SetImporter( m_meshe_import );
    m_chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
  
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    

    m_chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();



    
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );


    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );

    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    m_chunk->GetNodeFromPass( m_texturemirrorpass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->GetTexture( 0 )->LoadFromFile();

    m_chunk->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "reflector_pos", 24 );
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "reflector_normale", 25 );
    

    
    m_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "chunk" ) );
    m_chunk_node->SetContent( m_chunk );

    m_scenenodegraph.RegisterNode( m_chunk_node );

    DrawSpace::Dynamics::Body::Parameters cube_params;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;

    cube_params.initial_attitude.Translation( 0.0, 3.5, 0.0 );


    m_cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, cube_params ) );

    m_cube_body->Enable( false );

    m_cube_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "cube_body" ) );
    m_cube_body_node->SetContent( m_cube_body );

    m_scenenodegraph.AddNode( m_cube_body_node );
    m_scenenodegraph.RegisterNode( m_cube_body_node );

    m_chunk_node->LinkTo( m_cube_body_node );
    */
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_texcube2 = _DRAWSPACE_NEW_( Texture, Texture( "tc2" ));
    m_texcube2->SetPurpose( Texture::PURPOSE_COLOR );
    m_texcube2->SetFormat( 256, 256, 4 );


    m_cube2 = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_cube2->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_cube2->RegisterPassSlot( m_texturepass );
    m_cube2->RegisterPassSlot( m_texturemirrorpass );

        
    m_cube2->GetMeshe()->SetImporter( m_meshe_import );
    m_cube2->GetMeshe()->LoadFromFile( "object.ac", 0 );


    m_cube2->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
   

    //m_cube2->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "saturnmap.jpg" ) ), 0 );
    //m_cube2->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    m_cube2->GetNodeFromPass( m_texturepass )->SetTexture( m_texcube2, 0 );


    

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );



    //m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "saturnmap.jpg" ) ), 0 );
    //m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetTexture( 0 )->LoadFromFile();

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetTexture( m_texcube2, 0 );

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "reflector_pos", 24 );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "reflector_normale", 25 );

    



    m_cube2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "cube2" ) );
    m_cube2_node->SetContent( m_cube2 );

    m_scenenodegraph.RegisterNode( m_cube2_node );




    DrawSpace::Dynamics::Body::Parameters cube2_params;
    cube2_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube2_params.mass = 0.0;
    cube2_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
   

    m_cube2_collider = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Collider, DrawSpace::Dynamics::Collider );

    m_cube2_collider->SetKinematic( cube2_params );
    m_cube2_collider->AddToWorld( &m_world );


    m_cube2_colider_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Collider>, SceneNode<DrawSpace::Dynamics::Collider>( "cube2_body" ) );
    m_cube2_colider_node->SetContent( m_cube2_collider );
    
    m_scenenodegraph.RegisterNode( m_cube2_colider_node );

    m_cube2_node->LinkTo( m_cube2_colider_node );



    m_freemove.Init( Vector( 0.8, 1.1, 0.3, 1.0 ) );
    m_freemove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FreeMovement>, SceneNode<DrawSpace::Core::FreeMovement>( "freemvt_node" ) );
    m_freemove_node->SetContent( &m_freemove );


    m_scenenodegraph.AddNode( m_freemove_node );
    m_scenenodegraph.RegisterNode( m_freemove_node );
    
    m_cube2_colider_node->LinkTo( m_freemove_node );


}

void MainLoopService::create_ground( void )
{

    m_ground = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ground->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_ground->RegisterPassSlot( m_texturepass );
    m_ground->RegisterPassSlot( m_bumppass );
    
    m_ground->GetMeshe()->SetImporter( m_meshe_import );

    m_ground->GetMeshe()->LoadFromFile( "water.ac", 0 );


    m_ground->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
 
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.vso", true ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.pso", true ) ) );

    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    /*
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    */

    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    m_ground->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "color", 0 );
    m_ground->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "color", Vector( 1.0, 0.0, 1.0, 1.0 ) );


    m_ground->GetNodeFromPass( m_bumppass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_bumppass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_bump.vso", true ) ) );
    m_ground->GetNodeFromPass( m_bumppass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_bump.pso", true ) ) );
    m_ground->GetNodeFromPass( m_bumppass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_bumppass )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_ground->GetNodeFromPass( m_bumppass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_bumppass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    m_ground->GetNodeFromPass( m_bumppass )->AddShaderParameter( 1, "worldview", 0 );

    m_ground->GetNodeFromPass( m_bumppass )->SetTexture( m_wavespass->GetTargetTexture(), 0 );



    m_ground_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "ground" ) );
    m_ground_node->SetContent( m_ground );

    m_scenenodegraph.RegisterNode( m_ground_node );


    DrawSpace::Dynamics::Body::Parameters ground_params;
    ground_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 100.0, 0.0, 100.0, 1.0 );
    ground_params.mass = 0.0;
    ground_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;


    ground_params.initial_attitude.Translation( 0.0, -5.0, 0.0 );
    //ground_params.initial_attitude.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( 22.0 ) );

    m_ground_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, ground_params ) );
    m_ground_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "ground_body" ) );
    m_ground_body_node->SetContent( m_ground_body );

    m_scenenodegraph.AddNode( m_ground_body_node );
    m_scenenodegraph.RegisterNode( m_ground_body_node );

    m_ground_node->LinkTo( m_ground_body_node );



}

void MainLoopService::on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id )
{
    if( "Trigger" == p_widget_id )
    {
        //m_cube_body->Enable( true );
    }
    else if( "Quit" == p_widget_id )
    {
        (*m_closeapp_cb)( 0 );
    }
    else if( "Button_Create" )
    {
        create_dynamic_cube();
    }
}
