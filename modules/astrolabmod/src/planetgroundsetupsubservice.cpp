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

#include "planetgroundsetupsubservice.h"
#include "mainloopservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER(logger, "planetgroundsetupsubservice", NULL)

#define LAYOUT_FILE "planetgroundsetup.layout"

PlanetGroundSetupSubService::PlanetGroundSetupSubService( void ) :
m_planetconfig( NULL )
{
	m_guiwidgetpushbuttonclicked_cb = _DRAWSPACE_NEW_(GUIWidgetPushButtonClickedCallback, GUIWidgetPushButtonClickedCallback(this, &PlanetGroundSetupSubService::on_guipushbutton_clicked));
}
    
PlanetGroundSetupSubService::~PlanetGroundSetupSubService( void )
{
    _DRAWSPACE_DELETE_( m_guiwidgetpushbuttonclicked_cb );
}


void PlanetGroundSetupSubService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
}

void PlanetGroundSetupSubService::Init( DrawSpace::Logger::Configuration* p_logconf,
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{
    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    _DSDEBUG( logger, dsstring("PlanetSetup sub service : startup...") );

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );

    m_renderer->GUI_LoadLayout( LAYOUT_FILE );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Close_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Renderer" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_PlanetRay" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_PlanetRay" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_PlanetRay" );
    
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_PlainsAmplitude" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_PlainsAmplitude" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_PlainsAmplitude" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_MountainsAmplitude" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_MountainsAmplitude" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_MountainsAmplitude" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_MountainsOffset" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_MountainsOffset" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_MountainsOffset" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_VerticalOffset" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_VerticalOffset" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_VerticalOffset" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_BeachLimit" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_BeachLimit" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_BeachLimit" );
   
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_LandscapeBumpFactor" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_LandscapeBumpFactor" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_LandscapeBumpFactor" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_ZBufferActivationRelativeAlt" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_ZBufferActivationRelativeAlt" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_ZBufferActivationRelativeAlt" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_SplatTransitionUpRelativeAlt" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_SplatTransitionUpRelativeAlt" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_SplatTransitionUpRelativeAlt" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_SplatTransitionDownRelativeAlt" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_SplatTransitionDownRelativeAlt" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_SplatTransitionDownRelativeAlt" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_SplatTextureResol" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_SplatTextureResol" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_SplatTextureResol" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_GravityEnabled" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Checkbox_GravityEnabled" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_NbLODFreeCameras" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_NbLODFreeCameras" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_NbLODFreeCameras" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_NbLODInertBodies" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_NbLODInertBodies" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_NbLODInertBodies" );

    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );
}


void PlanetGroundSetupSubService::Run(void)
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

void PlanetGroundSetupSubService::Release( void )
{
    _DSDEBUG( logger, dsstring("PlanetGroundSetup sub service : shutdown...") );
}

DrawSpace::Core::BaseSceneNode* PlanetGroundSetupSubService::InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler )
{
    return NULL;
}

void PlanetGroundSetupSubService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetGroundSetupSubService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetGroundSetupSubService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
}

void PlanetGroundSetupSubService::OnKeyPress(long p_key)
{
    m_renderer->GUI_OnKeyDown( p_key );
}

void PlanetGroundSetupSubService::OnEndKeyPress(long p_key)
{
    m_renderer->GUI_OnKeyUp( p_key );
}

void PlanetGroundSetupSubService::OnKeyPulse(long p_key)
{
}

void PlanetGroundSetupSubService::OnChar(long p_char, long p_scan)
{
    m_renderer->GUI_OnChar( p_char );
}

void PlanetGroundSetupSubService::OnMouseMove(long p_xm, long p_ym, long p_dx, long p_dy)
{
    m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void PlanetGroundSetupSubService::OnMouseWheel(long p_delta)
{
}

void PlanetGroundSetupSubService::OnMouseLeftButtonDown(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseLeftButtonDown();
}

void PlanetGroundSetupSubService::OnMouseLeftButtonUp(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseLeftButtonUp();
}

void PlanetGroundSetupSubService::OnMouseRightButtonDown(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseRightButtonDown();
}

void PlanetGroundSetupSubService::OnMouseRightButtonUp(long p_xm, long p_ym)
{
    m_renderer->GUI_OnMouseRightButtonUp();
}

void PlanetGroundSetupSubService::OnAppEvent(WPARAM p_wParam, LPARAM p_lParam)
{
}

void PlanetGroundSetupSubService::on_guipushbutton_clicked(const dsstring& p_layout, const dsstring& p_widget_id)
{
    if( p_layout != LAYOUT_FILE )
    {
        return;
    }

    if( "Close_Button" == p_widget_id )
    {
		MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETGROUNDSETUP_CLOSEBUTTON_CLIC );
    }

    if( "Button_PlanetRay" == p_widget_id )
    {
    
    }

    if( "Button_PlainsAmplitude" == p_widget_id )
    {
    
    }

    if( "Button_MountainsAmplitude" == p_widget_id )
    {
    
    }

    if( "Button_MountainsOffset" == p_widget_id )
    {
    
    }

    if( "Button_VerticalOffset" == P_tmpdir )
    {
    
    }

    if( "Button_BeachLimit" == p_widget_id )
    {
    
    }

    if( "Button_LandscapeBumpFactor" == p_widget_id )
    {
    
    }

    if( "Button_ZBufferActivationRelativeAlt" == p_widget_id )
    {
    
    }

    if( "Button_SplatTransitionUpRelativeAlt" == p_widget_id )
    {
    
    }

    if( "Button_SplatTransitionDownRelativeAlt" == p_widget_id )
    {
    
    }

    if( "SimpleLabel_SplatTextureResol" == p_widget_id )
    {
    
    }

    if( "SimpleLabel_NbLODFreeCameras" == p_widget_id )
    {
    
    }

    if( "SimpleLabel_NbLODInertBodies" == p_widget_id )
    {
    
    }

}

void PlanetGroundSetupSubService::Activate( PlanetSceneNodeConfig* p_planetConfig )
{
    m_renderer->GUI_SetLayout( LAYOUT_FILE );

	m_planetconfig = p_planetConfig;

	// update local copy of config
	m_planetRay = m_planetconfig->m_planetRay.m_value;
}

void PlanetGroundSetupSubService::Unactivate( void )
{
}
