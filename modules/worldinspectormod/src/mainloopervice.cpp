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

_DECLARE_DS_LOGGER( logger, "worldinspectormainloopservice", NULL )

MainLoopService::MainLoopService( void )
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
    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );

    create_passes();
    init_passes();

    //m_renderer->GUI_InitSubSystem();

    _DSDEBUG( logger, dsstring("main loop service : startup...") );
}

void MainLoopService::Run( void )
{   
    /*
    m_renderer->BeginScreen();

    m_renderer->ClearScreen( 0, 255, 0, 255 );

    m_renderer->DrawText( 255, 0, 0, 10, 20, "%d fps - %s", m_tm.GetFPS(), m_pluginDescr.c_str() );

    m_renderer->EndScreen();

    m_renderer->FlipScreen();
    */

    m_texturepass->GetRenderingQueue()->Draw();
    m_finalpass->GetRenderingQueue()->Draw();

    m_renderer->DrawText( 255, 0, 0, 10, 20, "%d fps -- %s", m_tm.GetFPS(), m_pluginDescr.c_str() );

    //m_renderer->GUI_Render();


    m_renderer->FlipScreen();
    
    m_tm.Update();
    if( m_tm.IsReady() )
    {
    }
}

void MainLoopService::Release( void )
{
    _DSDEBUG( logger, dsstring("main loop service : shutdown...") );
}

void MainLoopService::OnKeyPress( long p_key )
{
}

void MainLoopService::OnEndKeyPress( long p_key )
{
}

void MainLoopService::OnKeyPulse( long p_key )
{
}

void MainLoopService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
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
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 128, 0, 128, 0 );


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