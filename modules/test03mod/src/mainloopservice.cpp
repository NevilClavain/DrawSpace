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
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "test01mainloopservice", NULL )

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

    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    /////////////////////////////////////////////////////////////////////////////////

    m_finalpass = m_rendergraph.CreateRoot( "final_pass" );

    m_finalpass.CreateViewportQuad( -0.24 );

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



    m_finalpass.GetRenderingQueue()->SetTargetClearingColor( 255, 25, 2, 255 );

    m_finalpass.GetRenderingQueue()->EnableDepthClearing( false );
    m_finalpass.GetRenderingQueue()->EnableTargetClearing( true );




    m_texturepass = m_finalpass.CreateChild( "texture_pass", 0 );

    m_texturepass.GetRenderingQueue()->SetTargetClearingColor( 0, 0, 200, 255 );
    m_texturepass.GetRenderingQueue()->EnableDepthClearing( false );
    m_texturepass.GetRenderingQueue()->EnableTargetClearing( true );



    m_rendergraph.RenderingQueueModSignal();
    
    
    
    m_rootEntity.AddAspect<TextRenderingAspect>();
    TextRenderingAspect* textrenderingAspect = m_rootEntity.GetAspect<TextRenderingAspect>();
    textrenderingAspect->AddComponent<DrawSpace::Core::TextRenderingAspect::TextDisplay>( "fps", 10, 10, 0, 255, 0, "" );


    m_cubeEntity.AddAspect<MesheRenderingAspect>();
    MesheRenderingAspect* mesherenderingaspect = m_cubeEntity.GetAspect<MesheRenderingAspect>();

    m_cubeEntity.AddAspect<TextRenderingAspect>();
    textrenderingAspect = m_rootEntity.GetAspect<TextRenderingAspect>();
    textrenderingAspect->AddComponent<DrawSpace::Core::TextRenderingAspect::TextDisplay>( "cube_text", 10, 30, 0, 255, 0, "Hello from cube entity!" );

    ///////////////////////////////////////////////////////////////////////////

    m_rootEntityNode = m_entitygraph.SetRoot( &m_rootEntity );
    m_cubeEntityNode = m_rootEntityNode.AddChild( &m_cubeEntity );

    

    


    _DSDEBUG( logger, dsstring("main loop service : startup...") );
}

void MainLoopService::Run( void )
{
    m_renderingSystem.Run( &m_rendergraph, &m_entitygraph );

    m_renderer->FlipScreen();

    if( m_display_switch )
    {
        char comment[256];
        sprintf( comment, "%d fps - %s", m_tm.GetFPS(), m_pluginDescr.c_str() );
        m_rootEntity.GetAspect<TextRenderingAspect>()->GetComponent<TextRenderingAspect::TextDisplay>( "fps" )->getPurpose().m_text = comment;
    }

    m_tm.Update();
    if( m_tm.IsReady() )
    {
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
}

void MainLoopService::OnEndKeyPress( long p_key )
{
}

void MainLoopService::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:
            {
                
                if( m_display_switch )
                {
                    TextRenderingAspect* textrenderingAspect = m_rootEntity.GetAspect<TextRenderingAspect>();
                    textrenderingAspect->RemoveComponent<TextRenderingAspect::TextDisplay>( "fps");

                    m_display_switch = false;
                }
                else
                {
                    TextRenderingAspect* textrenderingAspect = m_rootEntity.GetAspect<TextRenderingAspect>();
                    textrenderingAspect->AddComponent<TextRenderingAspect::TextDisplay>( "fps", 10, 10, 0, 255, 0, "Hello world !" );

                    m_display_switch = true;
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
}

void MainLoopService::OnMouseWheel( long p_delta )
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


