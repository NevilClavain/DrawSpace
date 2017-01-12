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

#include "planetviewsubservice.h"
#include "mainloopservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "planetviewsubservice", NULL )

#define LAYOUT_FILE "planetview.layout"

PlanetViewSubService::PlanetViewSubService( void )
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

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->GUI_LoadLayout( LAYOUT_FILE );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", 2 );
    
    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );
    m_renderer->GUI_SubscribeWidgetPushButtonEventClicked( LAYOUT_FILE, "Close_Button" );
}


void PlanetViewSubService::Run( void )
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
}

void PlanetViewSubService::OnChar( long p_char, long p_scan )
{
    m_renderer->GUI_OnChar( p_char );
}

void PlanetViewSubService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void PlanetViewSubService::OnMouseWheel( long p_delta )
{
}

void PlanetViewSubService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseLeftButtonDown();
}

void PlanetViewSubService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseLeftButtonUp();
}

void PlanetViewSubService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_renderer->GUI_OnMouseRightButtonDown();
}

void PlanetViewSubService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
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
        MainLoopService::GetInstance()->SetPlanetSetupLayout();
    }
}

void PlanetViewSubService::ApplyLayout( void )
{
    m_renderer->GUI_SetLayout( LAYOUT_FILE );
}