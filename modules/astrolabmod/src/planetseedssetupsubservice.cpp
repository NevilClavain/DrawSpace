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

#include "planetseedssetupsubservice.h"
#include "mainloopservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER(logger, "planetseedssetupsubservice", NULL)

#define LAYOUT_FILE "planetseedssetup.layout"

PlanetSeedsSetupSubService::PlanetSeedsSetupSubService( void ) :
m_planetconfig( NULL ),
m_statusbar_timer( LAYOUT_FILE, "Label_Status" )
{
	m_guiwidgetpushbuttonclicked_cb = _DRAWSPACE_NEW_( GUIWidgetPushButtonClickedCallback, GUIWidgetPushButtonClickedCallback( this, &PlanetSeedsSetupSubService::on_guipushbutton_clicked ) );
}
    
PlanetSeedsSetupSubService::~PlanetSeedsSetupSubService( void )
{
    _DRAWSPACE_DELETE_( m_guiwidgetpushbuttonclicked_cb );
}


void PlanetSeedsSetupSubService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
}

void PlanetSeedsSetupSubService::Init( DrawSpace::Logger::Configuration* p_logconf,
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

    m_renderer->GUI_LoadLayout( LAYOUT_FILE );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Close_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Renderer" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_PlanetName" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Status" );
    /*
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_PlanetRay" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_PlanetRay" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_PlanetRay" );
    */


    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );

    m_statusbar_timer.Init( m_tm );
}


void PlanetSeedsSetupSubService::Run(void)
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

void PlanetSeedsSetupSubService::Release( void )
{
    _DSDEBUG( logger, dsstring( "PlanetSeedsSetup sub service : shutdown..." ) );
}

DrawSpace::Core::BaseSceneNode* PlanetSeedsSetupSubService::InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler )
{
    return NULL;
}

void PlanetSeedsSetupSubService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetSeedsSetupSubService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetSeedsSetupSubService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
}

void PlanetSeedsSetupSubService::OnKeyPress(long p_key)
{
    m_renderer->GUI_OnKeyDown( p_key );
}

void PlanetSeedsSetupSubService::OnEndKeyPress(long p_key)
{
    m_renderer->GUI_OnKeyUp( p_key );
}

void PlanetSeedsSetupSubService::OnKeyPulse(long p_key)
{
}

void PlanetSeedsSetupSubService::OnChar(long p_char, long p_scan)
{
    m_renderer->GUI_OnChar( p_char );
}

void PlanetSeedsSetupSubService::OnMouseMove(long p_xm, long p_ym, long p_dx, long p_dy)
{
    m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void PlanetSeedsSetupSubService::OnMouseWheel(long p_delta)
{
}

void PlanetSeedsSetupSubService::OnMouseLeftButtonDown(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseLeftButtonDown();
}

void PlanetSeedsSetupSubService::OnMouseLeftButtonUp(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseLeftButtonUp();
}

void PlanetSeedsSetupSubService::OnMouseRightButtonDown(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseRightButtonDown();
}

void PlanetSeedsSetupSubService::OnMouseRightButtonUp(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseRightButtonUp();
}

void PlanetSeedsSetupSubService::OnAppEvent(WPARAM p_wParam, LPARAM p_lParam)
{
}

void PlanetSeedsSetupSubService::on_guipushbutton_clicked(const dsstring& p_layout, const dsstring& p_widget_id)
{
    dsstring text;

    if( p_layout != LAYOUT_FILE )
    {
        return;
    }

    if( "Close_Button" == p_widget_id )
    {
		MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETSEEDSSETUP_CLOSEBUTTON_CLIC );
    }
    /*
    if( "Button_PlanetRay" == p_widget_id )
    {
        m_renderer->GUI_GetWidgetText( LAYOUT_FILE, "Editbox_PlanetRay", text );

        try
        {
            m_planetconfig->m_planetRay = (dsreal)std::stoi( text );
            update_screen();
        }
        catch( std::invalid_argument )
        {
            m_statusbar_timer.Print( "bad value input!" );
            update_screen();
        }
    }
    */
  
}


void PlanetSeedsSetupSubService::Activate( PlanetSceneNodeConfig* p_planetConfig )
{
    m_renderer->GUI_SetLayout( LAYOUT_FILE );
	m_planetconfig = p_planetConfig;

    update_screen();
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_PlanetName", m_planetconfig->m_planetName.m_value );
}

void PlanetSeedsSetupSubService::Unactivate( void )
{
}

void PlanetSeedsSetupSubService::update_screen( void )
{
    char comment[64];
    char comment2[64];
    /*
    sprintf( comment, "%d km", (int)m_planetconfig->m_planetRay.m_value );
    sprintf( comment2, "%d", (int)m_planetconfig->m_planetRay.m_value );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "SimpleLabel_PlanetRay", comment );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_PlanetRay", comment2 );
    */

 
}