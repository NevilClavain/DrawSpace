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

_DECLARE_DS_LOGGER( logger, "guilabmainloopservice", NULL )

MainLoopService::MainLoopService( void )
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
    m_mousecircularmode_cb = p_mousecircularmode_cb;

    // hide OS mouse cursor (we use CEGUI mouse cursor instead)
    (*p_mousevisible_cb)( false );


    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    /////////////////////////////////////////////////////////////////////////////////

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->GUI_InitSubSystem();

    m_renderer->GUI_SetResourcesRootDirectory( "./xfskin2" );

    m_renderer->GUI_LoadScheme( "xfskin.scheme" );

    m_renderer->GUI_SetMouseCursorImage( "xfskin/MouseCursor" );

    
    m_renderer->GUI_LoadLayout( "test.layout" );
    m_renderer->GUI_SetLayout( "test.layout" );
   
    m_renderer->GUI_StoreWidget( "test.layout", "root", 1 );
    m_renderer->GUI_StoreWidget( "test.layout", "root", 2 );
    m_renderer->GUI_StoreWidget( "test.layout", "root", 3 );
    m_renderer->GUI_StoreWidget( "test.layout", "root", 4 );
    m_renderer->GUI_StoreWidget( "test.layout", "root", 5 );
    m_renderer->GUI_StoreWidget( "test.layout", "root", 6 );
    m_renderer->GUI_StoreWidget( "test.layout", "root", 7 );
    m_renderer->GUI_StoreWidget( "test.layout", "root", 8 );

    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );


    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( "test.layout", "Button_ClearText" );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( "test.layout", "Button_CopyInput" );

    _DSDEBUG( logger, dsstring("main loop service : startup...") );
}

void MainLoopService::set_mouse_circular_mode( bool p_state )
{
    if( m_mousecircularmode_cb )
    {
        (*m_mousecircularmode_cb)( p_state );

        m_renderer->GUI_ShowMouseCursor( !p_state );
    }
}


void MainLoopService::Run( void )
{   
    m_renderer->BeginScreen();

    m_renderer->ClearScreen( 0, 0, 0, 255 );

    m_renderer->DrawText( 255, 0, 0, 10, 20, "%d fps - %s", m_tm.GetFPS(), m_pluginDescr.c_str() );

    m_renderer->EndScreen();


    char comment[256];
    sprintf( comment, "%d fps - %s", m_tm.GetFPS(), m_pluginDescr.c_str() );
    dsstring fps_text = comment;
    //m_renderer->GUI_SetWidgetText( 1, fps_text );

    m_renderer->GUI_SetWidgetText( "test.layout", "Label", fps_text );

    m_renderer->GUI_Render();

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

DrawSpace::Core::BaseSceneNode* MainLoopService::InstanciateSceneNode( const dsstring& p_sceneNodeName )
{
    return NULL;
}

void MainLoopService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void MainLoopService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void MainLoopService::ReleaseSceneNode( const dsstring& p_sceneNodeName )
{
}

void MainLoopService::OnKeyPress( long p_key )
{
    m_renderer->GUI_OnKeyDown( p_key );
}

void MainLoopService::OnEndKeyPress( long p_key )
{
    m_renderer->GUI_OnKeyUp( p_key );
}

void MainLoopService::OnKeyPulse( long p_key )
{
    //m_renderer->GUI_OnChar( p_key );
}

void MainLoopService::OnChar( long p_char, long p_scan )
{
    m_renderer->GUI_OnChar( p_char );
}

void MainLoopService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void MainLoopService::OnMouseWheel( long p_delta )
{
}

void MainLoopService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseLeftButtonDown();
}

void MainLoopService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseLeftButtonUp();
}

void MainLoopService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseRightButtonDown();
}

void MainLoopService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseRightButtonUp();
}

void MainLoopService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void MainLoopService::on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id )
{
    if( "Button_ClearText" == p_widget_id )
    {
        //m_renderer->GUI_SetWidgetText( 3, "" );

        m_renderer->GUI_SetWidgetText( "test.layout", "Editbox", "" );
    }
    else if( "Button_CopyInput" == p_widget_id )
    {
        dsstring widget_text;
        m_renderer->GUI_GetWidgetText( "test.layout", "Editbox", widget_text );
        m_renderer->GUI_SetWidgetText( "test.layout", "Label2", widget_text );
    }
}

