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


#include "mainservice.h"
#include "component.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Utils;

_DECLARE_DS_LOGGER( logger, "service_example_mainservice", NULL )

MainService::MainService( void )
{
}

bool MainService::Init( void )
{


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

    m_systemsHub.Init( &m_entitygraph );

    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    /////////////////////////////////////////////////////////////////////////////////

    m_finalpass = m_rendergraph.CreateRoot( "final_pass" );

    m_finalpass.CreateViewportQuad();

    m_finalpass.GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );

    RenderStatesSet finalpass_rss;
    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "point" ) );
    finalpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );


    m_finalpass.GetViewportQuad()->GetFx()->SetRenderStates( finalpass_rss );


    m_finalpass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_finalpass.GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_finalpass.GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass.GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_finalpass.GetRenderingQueue()->EnableDepthClearing( false );
    m_finalpass.GetRenderingQueue()->EnableTargetClearing( false );


    m_texturepass = m_finalpass.CreateChild( "texture_pass", 0 );    
    m_texturepass.GetRenderingQueue()->EnableDepthClearing( false );
    m_texturepass.GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass.GetRenderingQueue()->SetTargetClearingColor( 90, 90, 90, 255 );

    ////////////////////////////////////////////////////////////////////////////////////

    RenderingAspect* rendering_aspect = m_rootEntity.AddAspect<RenderingAspect>();

    rendering_aspect->AddImplementation( &m_passesRender );
    m_passesRender.SetRendergraph( &m_rendergraph );

    rendering_aspect->AddImplementation( &m_textRender );
    rendering_aspect->AddComponent<TextRenderingAspectImpl::TextDisplay>( "fps", 10, 20, 255, 255, 255, "..." );

    TimeAspect* time_aspect = m_rootEntity.AddAspect<TimeAspect>();

    time_aspect->AddComponent<TimeManager>( "time_manager" );
    time_aspect->AddComponent<TimeAspect::TimeScale>( "time_scale", TimeAspect::NORMAL_TIME );
    time_aspect->AddComponent<dsstring>( "output_formated_datetime", "..." );
    time_aspect->AddComponent<dstime>( "time", 0 );
    time_aspect->AddComponent<int>( "output_fps" );
    time_aspect->AddComponent<int>( "output_world_nbsteps" );

    time_aspect->AddComponent<dsreal>( "output_time_factor" );

    m_rootEntityNode = m_entitygraph.SetRoot( &m_rootEntity );

    /////////////////////////////////////////////////////////////////////////////////////



    m_rendergraph.PushSignal_UpdatedRenderingQueue();
    m_entitygraph.PushSignal_RenderSceneBegin();



    set_mouse_circular_mode( false );

    _DSDEBUG( logger, dsstring("MainService : startup...") );

    return true;
}


void MainService::Run( void )
{
    m_systemsHub.Run( &m_entitygraph );
    
    TimeAspect* time_aspect = m_rootEntity.GetAspect<TimeAspect>();
    char comment[256];

    sprintf( comment, "%d fps - %s", time_aspect->GetComponent<int>( "output_fps" )->getPurpose(), m_pluginDescr.c_str() );

    RenderingAspect* rendering_aspect = m_rootEntity.GetAspect<RenderingAspect>();
    rendering_aspect->GetComponent<TextRenderingAspectImpl::TextDisplay>( "fps" )->getPurpose().m_text = comment;
}

void MainService::Release( void )
{
    _DSDEBUG( logger, dsstring("MainService : shutdown...") );


    
    m_systemsHub.Release( &m_entitygraph );
}

void MainService::OnKeyPress( long p_key )
{
}

void MainService::OnEndKeyPress( long p_key )
{
}

void MainService::OnKeyPulse( long p_key )
{
}

void MainService::OnChar( long p_char, long p_scan )
{
}

void MainService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
}

void MainService::OnMouseWheel( long p_delta )
{
}

void MainService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
}

void MainService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
}

void MainService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
}

void MainService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
}

void MainService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}




void MainService::set_mouse_circular_mode( bool p_state )
{
    if( m_mousecircularmode_cb )
    {
        (*m_mousecircularmode_cb)( p_state );        
    }
}
