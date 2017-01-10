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

#include "planetsetupsubservice.h"
#include "mainloopservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "planetsetupsubservice", NULL )

#define LAYOUT_FILE "planetsetup.layout"

PlanetSetupSubService::PlanetSetupSubService( void )
{
    m_guiwidgetpushbuttonclicked_cb = _DRAWSPACE_NEW_( GUIWidgetPushButtonClickedCallback, GUIWidgetPushButtonClickedCallback( this, &PlanetSetupSubService::on_guipushbutton_clicked ) );
}
    
PlanetSetupSubService::~PlanetSetupSubService( void )
{
    _DRAWSPACE_DELETE_( m_guiwidgetpushbuttonclicked_cb );
}


void PlanetSetupSubService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
}

void PlanetSetupSubService::Init( DrawSpace::Logger::Configuration* p_logconf, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{
    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    _DSDEBUG( logger, dsstring("PlanetSetup sub service : startup...") );

    m_closeapp_cb = p_closeapp_cb;

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->GUI_LoadLayout( LAYOUT_FILE );
    m_renderer->GUI_SetLayout( LAYOUT_FILE );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", 2 );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", 3 );
    
    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( LAYOUT_FILE, "Quit_Button" );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( LAYOUT_FILE, "PlanetView_Button" );

   
}


void PlanetSetupSubService::Run( void )
{
    m_renderer->BeginScreen();

    m_renderer->ClearScreen( 0, 0, 0, 0 );

    m_renderer->GUI_Render();

    m_renderer->EndScreen();

    m_renderer->FlipScreen();
    
    m_tm.Update();
    if( m_tm.IsReady() )
    {
    }
}

void PlanetSetupSubService::Release( void )
{
    _DSDEBUG( logger, dsstring("PlanetSetup sub service : shutdown...") );
}

DrawSpace::Core::BaseSceneNode* PlanetSetupSubService::InstanciateSceneNode( const dsstring& p_sceneNodeName )
{
    return NULL;
}

void PlanetSetupSubService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetSetupSubService::ReleaseSceneNode( const dsstring& p_sceneNodeName )
{
}

void PlanetSetupSubService::OnKeyPress( long p_key )
{
    m_renderer->GUI_OnKeyDown( p_key );
}

void PlanetSetupSubService::OnEndKeyPress( long p_key )
{
    m_renderer->GUI_OnKeyUp( p_key );
}

void PlanetSetupSubService::OnKeyPulse( long p_key )
{
}

void PlanetSetupSubService::OnChar( long p_char, long p_scan )
{
    m_renderer->GUI_OnChar( p_char );
}

void PlanetSetupSubService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void PlanetSetupSubService::OnMouseWheel( long p_delta )
{
}

void PlanetSetupSubService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseLeftButtonDown();
}

void PlanetSetupSubService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseLeftButtonUp();
}

void PlanetSetupSubService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseRightButtonDown();
}

void PlanetSetupSubService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseRightButtonUp();
}

void PlanetSetupSubService::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{
}

void PlanetSetupSubService::on_guipushbutton_clicked( dsstring p_widget_id )
{
    if( "Quit_Button" == p_widget_id )
    {
        (*m_closeapp_cb)( 0 );
    }
    else if( "PlanetView_Button" == p_widget_id )
    {
        MainLoopService::GetInstance()->SetPlanetViewLayout();
    }
}

