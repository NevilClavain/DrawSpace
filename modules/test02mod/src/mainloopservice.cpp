/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "test02mainloopservice", NULL )

MainLoopService::MainLoopService( void ) : 
m_fpsmove( true ),
m_waves( 0.0 ),
m_waves_inc( true )
{
}

MainLoopService::~MainLoopService( void )
{
}

void MainLoopService::GetLocalKeys( const dsstring p_instanceName, std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{

}

void MainLoopService::GetGlobalKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{

}

void MainLoopService::Init( DrawSpace::Logger::Configuration* p_logconf, DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb )
{
    m_mousecircularmode_cb = p_mousecircularmode_cb;

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

    create_passes();
    create_spacebox();
    create_camera();
    create_cubes();
    create_ground();

    init_passes();

    m_scenenodegraph.SetCurrentCamera( "camera" );

    DrawSpace::Interface::Renderer::DeviceDescr dd;
    m_renderer->GetDeviceDescr( dd );

    m_device = dd.description;

    set_mouse_circular_mode( true );

    _DSDEBUG( logger, dsstring("main loop service : startup...") );
}

void MainLoopService::Run( void )
{   

    m_scenenodegraph.ComputeTransformations( m_tm );

    Matrix water_plane_mat;
    m_ground_body_node->GetFinalTransform( water_plane_mat );


    Vector reflectorPos( water_plane_mat( 3, 0 ), water_plane_mat( 3, 1 ), water_plane_mat( 3, 2 ), 1.0 );
    Vector reflectorNormale( 0.0, 1.0, 0.0, 1.0 );

    for( int i = 0; i < 6; i++ )
    {
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->SetShaderRealVector( "reflector_normale", reflectorNormale );
    }

    
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_pos", reflectorPos );
    m_chunk->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_normale", reflectorNormale );

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_pos", reflectorPos );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "reflector_normale", reflectorNormale );
    

    m_wavespass->GetRenderingQueue()->Draw();
    m_texturepass->GetRenderingQueue()->Draw();
    m_texturemirrorpass->GetRenderingQueue()->Draw();
    m_finalpass->GetRenderingQueue()->Draw();

    m_renderer->DrawText( 255, 0, 0, 10, 20, "%d fps - %s - %s", m_tm.GetFPS(), m_device.c_str(), m_pluginDescr.c_str() );

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

BaseSceneNode* MainLoopService::GetSceneNode( void )
{
    return NULL;
}

void MainLoopService::set_mouse_circular_mode( bool p_state )
{
    if( m_mousecircularmode_cb )
    {
        (*m_mousecircularmode_cb)( p_state );
    }
}

void MainLoopService::OnKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':

            m_fpsmove.SetSpeed( 20.0 );
            break;

        case 'W':

            m_fpsmove.SetSpeed( -20.0 );
            break;

        case VK_F3:

            m_cube_body->Enable( true );
            break;

    }
}

void MainLoopService::OnEndKeyPress( long p_key )
{
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
}

void MainLoopService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
	m_fpsmove.RotateYaw( - p_dx / 4.0, m_tm );
	m_fpsmove.RotatePitch( - p_dy / 4.0, m_tm );
}

void MainLoopService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
}

void MainLoopService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
}

void MainLoopService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
}

void MainLoopService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
}

void MainLoopService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void MainLoopService::create_passes( void )
{
    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->Initialize();
    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );


    m_texturemirrorpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texturemirror_pass" ) );

    m_texturemirrorpass->Initialize();
    m_texturemirrorpass->GetRenderingQueue()->EnableDepthClearing( true );


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

    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    

    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "point" ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );

    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_mask.vso", true ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "water_mask.pso", true ) ) );

    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    m_finalpass->GetViewportQuad()->SetTexture( m_texturemirrorpass->GetTargetTexture(), 1 );

    //m_finalpass->GetViewportQuad()->SetTexture( m_wavespass->GetTargetTexture(), 0 );
    
}

void MainLoopService::init_passes( void )
{
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturemirrorpass->GetRenderingQueue()->UpdateOutputQueue();
    m_wavespass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
}


void MainLoopService::create_spacebox( void )
{
    m_spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
    m_spacebox->RegisterPassSlot( m_texturepass );
    m_spacebox->RegisterPassSlot( m_texturemirrorpass );

    for( long i = 0; i < 6; i++ )
    {

        m_spacebox->GetNodeFromPass( m_texturepass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->GetShader( 0 )->LoadFromFile();
        m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->GetShader( 1 )->LoadFromFile();

        //m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        //m_spacebox->GetNodeFromPass( m_texturepass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    }  
    
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetOrderNumber( 200 );



    for( long i = 0; i < 6; i++ )
    {

        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );      

        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->GetShader( 0 )->LoadFromFile();
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->GetShader( 1 )->LoadFromFile();

        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
        //m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
        //m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    }   


    
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::FrontQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RearQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::TopQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::BottomQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::LeftQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturemirrorpass, Spacebox::RightQuad )->SetOrderNumber( 200 );

    for( int i = 0; i < 6; i++ )
    {
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->AddShaderParameter( 0, "reflector_pos", 24 );
        m_spacebox->GetNodeFromPass( m_texturemirrorpass, i )->AddShaderParameter( 0, "reflector_normale", 25 );
    }


    
    m_spacebox_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Spacebox>, SceneNode<DrawSpace::Spacebox>( "spacebox" ) );
    m_spacebox_node->SetContent( m_spacebox );

    
    m_scenenodegraph.RegisterNode( m_spacebox_node );


    m_spacebox_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "spacebox_transfo" ) );
    m_spacebox_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix spacebox_scale;
    spacebox_scale.Scale( 20.0, 20.0, 20.0 );
    m_spacebox_transfo_node->GetContent()->PushMatrix( spacebox_scale );

    m_scenenodegraph.AddNode( m_spacebox_transfo_node );
    m_scenenodegraph.RegisterNode( m_spacebox_transfo_node );
    m_spacebox_node->LinkTo( m_spacebox_transfo_node );
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

void MainLoopService::create_cubes( void )
{


    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_chunk->RegisterPassSlot( m_texturepass );
    m_chunk->RegisterPassSlot( m_texturemirrorpass );


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

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
   

    m_cube2->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "saturnmap.jpg" ) ), 0 );
    m_cube2->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.vso", true ) ) );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture_mirror.pso", true ) ) );

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );



    m_cube2->GetNodeFromPass( m_texturemirrorpass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "saturnmap.jpg" ) ), 0 );
    m_cube2->GetNodeFromPass( m_texturemirrorpass )->GetTexture( 0 )->LoadFromFile();

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