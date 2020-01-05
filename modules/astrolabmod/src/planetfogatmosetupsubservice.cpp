/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#include "planetfogatmosetupsubservice.h"
#include "mainloopservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER(logger, "planetfogatmosetupsubservice", NULL)

#define LAYOUT_FILE "planetfogatmosetup.layout"

PlanetFogAtmoSetupSubService::PlanetFogAtmoSetupSubService( void ) :
m_planetconfig( NULL ),
m_statusbar_timer( LAYOUT_FILE, "Label_Status" )
{
	m_guiwidgetpushbuttonclicked_cb = _DRAWSPACE_NEW_( GUIWidgetPushButtonClickedCallback, GUIWidgetPushButtonClickedCallback( this, &PlanetFogAtmoSetupSubService::on_guipushbutton_clicked ) );
    m_guiwidgetcheckboxstatechanged_cb = _DRAWSPACE_NEW_( GUIWidgetCheckboxStateChangedCallback, GUIWidgetCheckboxStateChangedCallback( this, &PlanetFogAtmoSetupSubService::on_guicheckboxstatechanged_clicked ) );
}
    
PlanetFogAtmoSetupSubService::~PlanetFogAtmoSetupSubService( void )
{
    _DRAWSPACE_DELETE_( m_guiwidgetpushbuttonclicked_cb );
}


void PlanetFogAtmoSetupSubService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
}

void PlanetFogAtmoSetupSubService::Init( DrawSpace::Logger::Configuration* p_logconf,
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{
    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    _DSDEBUG( logger, dsstring( "PlanetSeedsSetup sub service : startup..." ) );

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->GUI_LoadLayout( LAYOUT_FILE, "xfskin2/layouts/planetfogatmosetup_widgets.conf" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Close_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Renderer" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_PlanetName" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Status" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_AtmoRenderEnable" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Checkbox_AtmoRenderEnable" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_AtmoKr" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_AtmoKr" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_AtmoKr" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_GroundFogAltLimit" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_GroundFogAltLimit" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_GroundFogAltLimit" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_GroundFogDensity" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_GroundFogDensity" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_GroundFogDensity" );


    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );
    m_renderer->GUI_RegisterCheckboxEventStateChangedHandler( m_guiwidgetcheckboxstatechanged_cb );


    m_statusbar_timer.Init( m_tm );
}


void PlanetFogAtmoSetupSubService::Run(void)
{
    m_renderer->BeginScreen();

    m_renderer->ClearScreen( 0, 0, 0, 0 );

    char renderer_name[64];

    sprintf( renderer_name, "%s", m_pluginDescr.c_str() );

    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_Renderer", renderer_name );

    m_renderer->GUI_Render();
    m_renderer->EndScreen();
    m_renderer->FlipScreen();
    
    m_tm.Update();
    if( m_tm.IsReady() )
    {
    }
}

void PlanetFogAtmoSetupSubService::Release( void )
{
    _DSDEBUG( logger, dsstring( "PlanetFogAtmoSetupSubService sub service : shutdown..." ) );
}

DrawSpace::Core::BaseSceneNode* PlanetFogAtmoSetupSubService::InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler )
{
    return NULL;
}

void PlanetFogAtmoSetupSubService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetFogAtmoSetupSubService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetFogAtmoSetupSubService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
}

void PlanetFogAtmoSetupSubService::OnKeyPress(long p_key)
{
    m_renderer->GUI_OnKeyDown( p_key );
}

void PlanetFogAtmoSetupSubService::OnEndKeyPress(long p_key)
{
    m_renderer->GUI_OnKeyUp( p_key );
}

void PlanetFogAtmoSetupSubService::OnKeyPulse(long p_key)
{
}

void PlanetFogAtmoSetupSubService::OnChar(long p_char, long p_scan)
{
    m_renderer->GUI_OnChar( p_char );
}

void PlanetFogAtmoSetupSubService::OnMouseMove(long p_xm, long p_ym, long p_dx, long p_dy)
{
    m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void PlanetFogAtmoSetupSubService::OnMouseWheel(long p_delta)
{
}

void PlanetFogAtmoSetupSubService::OnMouseLeftButtonDown(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseLeftButtonDown();
}

void PlanetFogAtmoSetupSubService::OnMouseLeftButtonUp(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseLeftButtonUp();
}

void PlanetFogAtmoSetupSubService::OnMouseRightButtonDown(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseRightButtonDown();
}

void PlanetFogAtmoSetupSubService::OnMouseRightButtonUp(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseRightButtonUp();
}

void PlanetFogAtmoSetupSubService::OnAppEvent(WPARAM p_wParam, LPARAM p_lParam)
{
}

void PlanetFogAtmoSetupSubService::on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id )
{
    dsstring text;

    if( p_layout != LAYOUT_FILE )
    {
        return;
    }

    if( "Close_Button" == p_widget_id )
    {
		MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETFOGATMOSETUP_CLOSEBUTTON_CLIC );
    }
    
    if( "Button_AtmoKr" == p_widget_id )
    {
        m_renderer->GUI_GetWidgetText( LAYOUT_FILE, "Editbox_AtmoKr", text );

        try
        {
            m_planetconfig->m_atmoKr = std::stof( text );
            update_screen();
        }
        catch( std::invalid_argument )
        {
            m_statusbar_timer.Print( "bad value input!" );
            update_screen();
        }
    }

    if( "Button_GroundFogAltLimit" == p_widget_id )
    {
        m_renderer->GUI_GetWidgetText( LAYOUT_FILE, "Editbox_GroundFogAltLimit", text );

        try
        {
            m_planetconfig->m_groundFogAltLimit = (dsreal)std::stoi( text );
            update_screen();
        }
        catch( std::invalid_argument )
        {
            m_statusbar_timer.Print( "bad value input!" );
            update_screen();
        }
    }

    if( "Button_GroundFogDensity" == p_widget_id )
    {
        m_renderer->GUI_GetWidgetText( LAYOUT_FILE, "Editbox_GroundFogDensity", text );

        try
        {
            m_planetconfig->m_groundFogDensity = std::stof( text );
            update_screen();
        }
        catch( std::invalid_argument )
        {
            m_statusbar_timer.Print( "bad value input!" );
            update_screen();
        }
    }
}

void PlanetFogAtmoSetupSubService::on_guicheckboxstatechanged_clicked( const dsstring& p_layout, const dsstring& p_widget_id, bool p_state )
{
    if( p_layout != LAYOUT_FILE )
    {
        return;
    }

    if( "Checkbox_AtmoRenderEnable" == p_widget_id )
    {
        m_planetconfig->m_atmoRenderEnable = p_state;
        update_screen();
    }
}

void PlanetFogAtmoSetupSubService::Activate( PlanetSceneNodeConfig* p_planetConfig )
{
    m_renderer->GUI_SetLayout( LAYOUT_FILE );
	m_planetconfig = p_planetConfig;

    update_screen();
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_PlanetName", m_planetconfig->m_planetName.m_value );
}

void PlanetFogAtmoSetupSubService::Unactivate( void )
{
}

void PlanetFogAtmoSetupSubService::update_screen( void )
{
    char comment[64];
    char comment2[64];

    m_renderer->GUI_SetCheckboxState( LAYOUT_FILE, "Checkbox_AtmoRenderEnable", m_planetconfig->m_atmoRenderEnable.m_value );
    sprintf( comment, "%s", m_planetconfig->m_atmoRenderEnable.m_value ? "true" : "false" );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "SimpleLabel_AtmoRenderEnable", comment );

    sprintf( comment, "%.4f", m_planetconfig->m_atmoKr.m_value );
    sprintf( comment2, "%.4f", m_planetconfig->m_atmoKr.m_value );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "SimpleLabel_AtmoKr", comment );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_AtmoKr", comment2 );

    sprintf( comment, "%d", (int)m_planetconfig->m_groundFogAltLimit.m_value );
    sprintf( comment2, "%d", (int)m_planetconfig->m_groundFogAltLimit.m_value );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "SimpleLabel_GroundFogAltLimit", comment );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_GroundFogAltLimit", comment2 );

    sprintf( comment, "%.5f", m_planetconfig->m_groundFogDensity.m_value );
    sprintf( comment2, "%.5f", m_planetconfig->m_groundFogDensity.m_value );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "SimpleLabel_GroundFogDensity", comment );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_GroundFogDensity", comment2 );
}
