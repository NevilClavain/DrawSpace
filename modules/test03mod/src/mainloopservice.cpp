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
    // BUILD FINALPASS



    m_Entity_finalpass.AddComponent<DrawSpace::Core::RenderingQueue>();
    m_Entity_finalpass.AddMultiComponent<RendergraphSystem::Text>();
    m_Entity_finalpass.AddComponent<RendergraphSystem::RenderingQueueStates>();
    m_Entity_finalpass.AddComponent<DrawSpace::ViewportQuad>();



    
    m_Entity_finalpass.RegisterSingleComponentAction<DrawSpace::Core::RenderingQueue>( RendergraphSystem::MakeRenderingQueueOnScreen );

    /*
    RendergraphSystem::RenderingQueueStates rq_states;

    rq_states.depth_clearing_enabled = false;
    rq_states.target_clearing_enabled = true;

    rq_states.target_clear_r = 50;
    rq_states.target_clear_g = 50;
    rq_states.target_clear_b = 50;
    rq_states.target_clear_a = 255;

    m_Entity_finalpass.RegisterSingleComponentAction<RendergraphSystem::RenderingQueueStates, RendergraphSystem::RenderingQueueStates>( RendergraphSystem::MakeRenderingQueueStatesOperation, rq_states );
    */

    m_Entity_finalpass.GetComponent<RendergraphSystem::RenderingQueueStates>()->MakePurpose();

     
    RendergraphSystem::RenderingQueueStates& rq_states = m_Entity_finalpass.GetComponent<RendergraphSystem::RenderingQueueStates>()->GetPurpose();
    rq_states.depth_clearing_enabled = false;
    rq_states.target_clearing_enabled = true;

    rq_states.target_clear_r = 150;
    rq_states.target_clear_g = 150;
    rq_states.target_clear_b = 150;
    rq_states.target_clear_a = 255;
    
    m_Entity_finalpass.RegisterDoubleComponentsAction<DrawSpace::Core::RenderingQueue, RendergraphSystem::RenderingQueueStates>( RendergraphSystem::SetRenderingQueueStates );

    /*
    DrawSpace::RendergraphSystem::Text simple_text;
    simple_text.r = 25;
    simple_text.g = 255;
    simple_text.b = 25;
    simple_text.x = 100;
    simple_text.y = 100;
    simple_text.text = "green text";

    m_Entity_finalpass.RegisterSingleComponentAction<RendergraphSystem::Text, RendergraphSystem::Text>( RendergraphSystem::MakeTextOperation, simple_text );

    m_text.r = 255;
    m_text.g = 0;
    m_text.b = 255;
    m_text.x = 10;
    m_text.y = 10;
    m_text.text = "test test test";

    m_Entity_finalpass.RegisterSingleComponentAction<RendergraphSystem::Text, RendergraphSystem::Text>( RendergraphSystem::MakeTextOperation, m_text );
    */

    m_Entity_finalpass.GetComponentMultiple<RendergraphSystem::Text>()->MakePurpose();
    m_Entity_finalpass.GetComponentMultiple<RendergraphSystem::Text>()->MakePurpose();

    RendergraphSystem::Text& text1 = m_Entity_finalpass.GetComponentMultiple<RendergraphSystem::Text>()->GetPurpose( 0 );
    text1.r = 25;
    text1.g = 255;
    text1.b = 25;
    text1.x = 100;
    text1.y = 100;
    text1.text = "green text";
        
    RendergraphSystem::Text& text2 = m_Entity_finalpass.GetComponentMultiple<RendergraphSystem::Text>()->GetPurpose( 1 );
    text2.r = 255;
    text2.g = 2;
    text2.b = 255;
    text2.x = 10;
    text2.y = 10;

    m_Entity_finalpass.RegisterSingleComponentAction<DrawSpace::Core::RenderingQueue>( RendergraphSystem::DrawRenderingQueue );
    m_Entity_finalpass.RegisterSingleComponentAction<RendergraphSystem::Text>( RendergraphSystem::DrawTextsOperation );


    m_Entity_finalpass.RegisterSingleComponentAction<DrawSpace::ViewportQuad>( RendergraphSystem::MakeViewportQuadAutoAdjustedOnScren );

    m_Entity_finalpass.RegisterDoubleComponentsAction<DrawSpace::ViewportQuad, DrawSpace::Core::RenderingQueue>( RendergraphSystem::SetViewportQuadOnRenderingQueue );

    m_Entity_finalpass.RegisterSingleComponentAction<DrawSpace::Core::RenderingQueue>( RendergraphSystem::UpdateRenderingQueue );

 
    m_Data_Rendergraph.AddRoot(&m_System_rendergraph, &m_Entity_finalpass);

/*

    ///////////////////////////////////////////////////////////////////////////////////

    
    m_Component_viewport_quad->m_viewportquad->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );


    RenderStatesSet finalpass_rss;


    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "point" ) );
    finalpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    //finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    //finalpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    m_Component_viewport_quad->m_viewportquad->GetFx()->SetRenderStates( finalpass_rss );

    m_Component_viewport_quad->m_viewportquad->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_Component_viewport_quad->m_viewportquad->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_Component_viewport_quad->m_viewportquad->GetFx()->GetShader( 0 )->LoadFromFile();
    m_Component_viewport_quad->m_viewportquad->GetFx()->GetShader( 1 )->LoadFromFile();

    //m_Component_viewport_quad->m_viewportquad->AddShaderParameter( 1, "color", 0 );
    //m_Component_viewport_quad->m_viewportquad->SetShaderRealVector( "color", Vector( 1.0, 0.5, 0.5, 1.0 ) );

    //m_Component_viewport_quad->m_viewportquad->SetTexture( m_Component_texturepass_render_target->m_targettexture, 0 );


    ///////////////////////////////////////////////////////////////////////////////////

    */

    /*

    m_Component_cube2_texturepass->m_rendering_node->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );


    m_Component_cube2_texturepass->m_rendering_node->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_Component_cube2_texturepass->m_rendering_node->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_Component_cube2_texturepass->m_rendering_node->GetFx()->GetShader( 0 )->LoadFromFile();
    m_Component_cube2_texturepass->m_rendering_node->GetFx()->GetShader( 1 )->LoadFromFile();


    RenderStatesSet chunk_rss;

    chunk_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    chunk_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    chunk_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    chunk_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );


    m_Component_cube2_texturepass->m_rendering_node->GetFx()->SetRenderStates( chunk_rss );



    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_Component_rendering_queue->m_queue->UpdateOutputQueue();
    m_Component_texturepass_rendering_queue->m_queue->UpdateOutputQueue();
    
    */



    _DSDEBUG( logger, dsstring("main loop service : startup...") );
}

void MainLoopService::Run( void )
{
    //m_Entity_finalpass.GetComponentMultiplePurpose<RendergraphSystem::Text>( 1 ).text = dsstring( "fps : " ) << m_tm.GetFPS() << dsstring( " - " ) <<  m_pluginDescr.c_str();

    m_Entity_finalpass.GetComponentMultiple<RendergraphSystem::Text>()->GetPurpose( 1 ).text = dsstring( "fps : " ) << m_tm.GetFPS() << dsstring( " - " ) <<  m_pluginDescr.c_str();

    m_Data_Rendergraph.AcceptSystemLeafsToTopRecursive( &m_System_rendergraph );

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


