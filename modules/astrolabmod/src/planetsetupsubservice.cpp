/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "planetsetupsubservice.h"
#include "mainloopservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER(logger, "planetsetupsubservice", NULL)

#define LAYOUT_FILE "planetsetup.layout"

PlanetSetupSubService::PlanetSetupSubService(void) :
m_selected_planet_conf( NULL ),
m_statusbar_timer( LAYOUT_FILE, "Label_Status" ),
m_rotation( 0.0 )
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

    m_renderer->GUI_LoadLayout( LAYOUT_FILE, "xfskin2/layouts/planetsetup_widgets.conf" );
    
    /*
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Quit_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "PlanetView_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Renderer" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_FPS" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Mem" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "AddPlanet_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "PlanetSlots_Listbox" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "PlanetName_Editbox" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Status" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SeedsSetup_Button" );
	m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "GroundSetup_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "ClimateSetup_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "WaterSetup_Button" );
	m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "AtmoSetup_Button" );
	m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "CloudsSetup_Button" );
	m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "ResourcesSetup_Button" );
    */

    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );


    /////////////////////////////////

    m_renderer->GUI_CreateSprite( "xfskin/StaticImage", "test_sprite" );
    m_renderer->GUI_SetSpriteImage( "test_sprite", "xfskin/ProgressIcon" );

    m_renderer->GUI_SetSpritePosition( "test_sprite", 0.94, 0.94 );

    m_renderer->GUI_SetSpriteScale( "test_sprite", 0.33 );

    /////////////////////////////////

    m_statusbar_timer.Init( m_tm );
}


void PlanetSetupSubService::Run( void )
{
    m_renderer->GUI_SetSpriteRotation( "test_sprite", DrawSpace::Utils::Vector( 0.0, 0.0, 1.0, 1.0 ), m_rotation );

    m_renderer->BeginScreen();

    m_renderer->ClearScreen( 0, 0, 0, 0 );

    char renderer_name[64];
    char fps[64];

    sprintf( fps, "%d fps", m_tm.GetFPS() );
    sprintf( renderer_name, "%s", m_pluginDescr.c_str() );

    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_FPS", fps );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_Renderer", renderer_name );

    char working_set[64];

    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof( PROCESS_MEMORY_COUNTERS ) );

    sprintf( working_set, "%.1f Mb", pmc.WorkingSetSize / 1000000.0 );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_Mem", working_set );


    m_renderer->GUI_Render();

    m_renderer->EndScreen();

    m_renderer->FlipScreen();


    
    m_tm.Update();
    if( m_tm.IsReady() )
    {
        m_tm.AngleSpeedDec( &m_rotation, 360.0 );
    }
}

void PlanetSetupSubService::Release( void )
{
    _DSDEBUG( logger, dsstring( "PlanetSetup sub service : shutdown..." ) );
}

DrawSpace::Core::BaseSceneNode* PlanetSetupSubService::InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler )
{
    return NULL;
}

void PlanetSetupSubService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetSetupSubService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetSetupSubService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
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
    switch( p_key )
    {
        case VK_F1:

            MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETSETUP_F1_KEY );
            break;    
    }
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

void PlanetSetupSubService::on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id )
{
    if( p_layout != LAYOUT_FILE )
    {
        return;
    }

    if( "Quit_Button" == p_widget_id )
    {
        MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETSETUP_QUITBUTTON_CLIC );
    }
    else if( "PlanetView_Button" == p_widget_id )
    {
        int lb_index;
        dsstring lb_text;

        bool select = m_renderer->GUI_GetListboxFirstSelectedItemIndex( LAYOUT_FILE, "PlanetSlots_Listbox", lb_index, lb_text );

        if( select )
        {
			m_selected_planet_conf = &m_nodes_config[lb_text];
            MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETSETUP_PLANETVIEWBUTTON_CLIC );
        }
        else
        {
            m_statusbar_timer.Print( "you must select a planet entry !" );
        }
    }
    else if( "AddPlanet_Button" == p_widget_id )
    {       
        dsstring node_name;
        m_renderer->GUI_GetWidgetText( LAYOUT_FILE, "PlanetName_Editbox", node_name );
        
        if( node_name != "" )
        {
            if( m_nodes_config.count( node_name ) )
            {
                m_statusbar_timer.Print( "planet entry with same name exists !" );
            }
            else
            {
                m_nodes_config[node_name].m_keylinksTable = m_cdlodp_service->AddSceneNodeConfig( node_name );

			    // connect keys here
			    m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_planetName );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_planetRay );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_plainsAmplitude );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_moutainsAmplitude );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_moutainsOffset );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_verticalOffset );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_beachLimit );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_landscapeBumpFactor );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_zbufferActivationRelAlt );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_splatTransitionUpRelativeAlt );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_splatTransitionDownRelativeAlt );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_nbLODFreeCameras );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_nbLODInertBodies );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_splatTextureResol );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_plainsSeed1 );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_plainsSeed2 );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_mixSeed1 );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_mixSeed2 );
			    m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_detailsVertexShader );
			    m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_detailsPixelShader );
			    m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_gravityEnabled );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_atmoRenderEnable );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_atmoKr );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_groundFogAltLimit );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_groundFogDensity );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_climatePassEnabled );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_skirtsEnabled );
                m_nodes_config[node_name].m_keylinksTable->RegisterClientKey( &m_nodes_config[node_name].m_landplacePatchEnabled );

			    // store planet unique name (also used as scenegraph node id)
			    m_nodes_config[node_name].m_planetName = node_name;
			    // setup default values here
                m_nodes_config[node_name].m_planetRay = 6500.0;

                m_nodes_config[node_name].m_plainsAmplitude = 600.0;//50.0;
                m_nodes_config[node_name].m_moutainsAmplitude = 16000.0;//800.0;
                m_nodes_config[node_name].m_moutainsOffset = 0;//-2200.0;
                m_nodes_config[node_name].m_verticalOffset = 20.0;
                m_nodes_config[node_name].m_beachLimit = 25.0;
				m_nodes_config[node_name].m_landscapeBumpFactor = 16.0;
                m_nodes_config[node_name].m_zbufferActivationRelAlt = 1.0098;
                m_nodes_config[node_name].m_splatTransitionUpRelativeAlt = 1.095;
                m_nodes_config[node_name].m_splatTransitionDownRelativeAlt = 1.0040;
                m_nodes_config[node_name].m_nbLODFreeCameras = 14;
                m_nodes_config[node_name].m_nbLODInertBodies = 14;
                m_nodes_config[node_name].m_splatTextureResol = 16;

                m_nodes_config[node_name].m_plainsSeed1 = 1.0;
                m_nodes_config[node_name].m_plainsSeed2 = 2.0;
                m_nodes_config[node_name].m_mixSeed1 = 3.0;
                m_nodes_config[node_name].m_mixSeed2 = 4.0;

			    //m_nodes_config[node_name].m_detailsVertexShader = "planet_surface_flat.vso";
			    //m_nodes_config[node_name].m_detailsPixelShader = "planet_surface_flat.pso";
                //m_nodes_config[node_name].m_climatePassEnabled = false;

			    m_nodes_config[node_name].m_detailsVertexShader = "planet_surface.vso";
			    m_nodes_config[node_name].m_detailsPixelShader = "planet_surface.pso";
                m_nodes_config[node_name].m_climatePassEnabled = true;

			    m_nodes_config[node_name].m_gravityEnabled = false;
                m_nodes_config[node_name].m_atmoRenderEnable = true;
                m_nodes_config[node_name].m_atmoKr = 0.0038;
                m_nodes_config[node_name].m_groundFogAltLimit = 30000.0;//10000.0;
                m_nodes_config[node_name].m_groundFogDensity = 0.000032;//0.00042;

                m_nodes_config[node_name].m_skirtsEnabled = true;
                m_nodes_config[node_name].m_landplacePatchEnabled = true;



                update_listbox();
            }
        }
        else
        {
            m_statusbar_timer.Print( "planet slot must have a name !" );
        }
    }
	else if( "GroundSetup_Button" == p_widget_id )
	{
		int lb_index;
		dsstring lb_text;

		bool select = m_renderer->GUI_GetListboxFirstSelectedItemIndex( LAYOUT_FILE, "PlanetSlots_Listbox", lb_index, lb_text );

		if (select)
		{
			m_selected_planet_conf = &m_nodes_config[lb_text];
			MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETSETUP_PLANETGROUNDSETUPBUTTON_CLIC );
		}
		else
		{
            m_statusbar_timer.Print( "you must select a planet entry !" );
		}
	}
	else if( "SeedsSetup_Button" == p_widget_id )
	{
		int lb_index;
		dsstring lb_text;

		bool select = m_renderer->GUI_GetListboxFirstSelectedItemIndex( LAYOUT_FILE, "PlanetSlots_Listbox", lb_index, lb_text );

		if (select)
		{
			m_selected_planet_conf = &m_nodes_config[lb_text];
			MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETSETUP_PLANETSEEDSSETUPBUTTON_CLIC );
		}
		else
		{
            m_statusbar_timer.Print( "you must select a planet entry !" );
		}
	}
	else if( "AtmoSetup_Button" == p_widget_id )
	{
		int lb_index;
		dsstring lb_text;

		bool select = m_renderer->GUI_GetListboxFirstSelectedItemIndex( LAYOUT_FILE, "PlanetSlots_Listbox", lb_index, lb_text );

		if (select)
		{
			m_selected_planet_conf = &m_nodes_config[lb_text];
			MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETSETUP_PLANETFOGATMOSETUPBUTTON_CLIC );
		}
		else
		{
            m_statusbar_timer.Print( "you must select a planet entry !" );
		}
	}


}

void PlanetSetupSubService::Activate( void )
{
    m_renderer->GUI_SetLayout( LAYOUT_FILE );
}

void PlanetSetupSubService::Unactivate( void )
{
}

void PlanetSetupSubService::SetCDLODInfos( DrawSpace::Interface::Module::Root* p_cdlodp_root, DrawSpace::Interface::Module::Service* p_cdlodp_service )
{
    m_cdlodp_root = p_cdlodp_root;
    m_cdlodp_service = p_cdlodp_service;

    connect_keys( m_cdlodp_service );
}

void PlanetSetupSubService::update_listbox( void )
{
    m_renderer->GUI_ClearListbox( LAYOUT_FILE, "PlanetSlots_Listbox" );

    for( auto it  = m_nodes_config.begin(); it != m_nodes_config.end(); ++it )
    {
        dsstring planetNodeName = it->first;

        m_renderer->GUI_AddListboxTextItem( LAYOUT_FILE, "PlanetSlots_Listbox", planetNodeName, 0xFF037574, "xfskin/GenericBrush" );
    }
}

PlanetSceneNodeConfig* PlanetSetupSubService::GetSelectedPlanetConfig(void)
{
	return m_selected_planet_conf;
}
