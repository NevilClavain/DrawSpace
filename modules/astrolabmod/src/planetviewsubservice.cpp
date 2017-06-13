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
#include "planetviewsubservice.h"
#include "mainloopservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "planetviewsubservice", NULL )

#define LAYOUT_FILE "planetview.layout"

PlanetViewSubService::PlanetViewSubService( void ) :
m_planet_node( NULL ),
m_mouse_left( false ),
m_mouse_right( false ),
m_cdlodplanet_scenenodegraph( "cdlodplanet.SceneNodeGraph" ),
m_cdlodplanet_texturepass( "cdlodplanet.TexturePass" ),
m_skybox_texturepass( "skybox.TexturePass" ),
m_skybox_texturemirrorpass( "skybox.TextureMirrorPass" ),
m_skybox_reflectornormale( "skybox.ReflectorNormale" ),
m_rel_altitude( 0.0 ),
m_planet_conf( NULL ),
m_mousewheel_delta( 0 ),
m_shift( false ),
m_ctrl( false )
{
    m_guiwidgetpushbuttonclicked_cb = _DRAWSPACE_NEW_( GUIWidgetPushButtonClickedCallback, GUIWidgetPushButtonClickedCallback( this, &PlanetViewSubService::on_guipushbutton_clicked ) );
    m_guiwidgetcheckboxstatechanged_cb = _DRAWSPACE_NEW_( GUIWidgetCheckboxStateChangedCallback, GUIWidgetCheckboxStateChangedCallback( this, &PlanetViewSubService::on_guicheckboxstatechanged_clicked ) );


    m_loddepnodeinfosstate_cb = _DRAWSPACE_NEW_( LODDependantNodeInfoStateCallback, LODDependantNodeInfoStateCallback( this, &PlanetViewSubService::on_LODdepnodeinfosstate_update ) );

    m_hotparams_list.push_back( "gravityEnabled" );
    m_hotparams_list.push_back( "landscapeBumpFactor" );
    m_hotparams_list.push_back( "atmoRenderEnable" );
    m_hotparams_list.push_back( "atmoKr" );
    m_hotparams_list.push_back( "groundFogAltLimit" );
    m_hotparams_list.push_back( "groundFogDensity" );
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
    m_keysLinkTable.RegisterClientKey( &m_skybox_texturepass );
    m_keysLinkTable.RegisterClientKey( &m_skybox_texturemirrorpass );
    m_keysLinkTable.RegisterClientKey( &m_skybox_reflectornormale );

    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    _DSDEBUG( logger, dsstring("PlanetView sub service : startup...") );

    m_closeapp_cb = p_closeapp_cb;

    m_keysLinkTable.RegisterClientKey( &m_cdlodplanet_scenenodegraph );
    m_keysLinkTable.RegisterClientKey( &m_cdlodplanet_texturepass );

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    m_renderer->GetDescr( m_pluginDescr );

	World::m_scale = 1.0;
    m_world.Initialize();

    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_tm ) );
    m_calendar->RegisterWorld( &m_world );

    create_passes();
    create_cubes();

    m_cdlodplanet_scenenodegraph = &m_scenenodegraph;
    m_cdlodplanet_texturepass = m_texturepass;

    m_skybox_texturepass = m_texturepass;

    create_spacebox();

    init_passes();

    m_renderer->GUI_LoadLayout( LAYOUT_FILE );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Close_Button" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Renderer" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_FPS" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_Mem" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Label_PlanetName" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_Relative" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_Altitude" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_SubPasses" );


    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "SimpleLabel_HotParamName" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Editbox_HotParam" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_HotParamUpdate" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Checkbox_HotParam" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_HotParamNext" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_HotParamPrev" );

    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_ResetPlanetTransform" );
    m_renderer->GUI_StoreWidget( LAYOUT_FILE, "root", "Button_ResetAll" );


    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_Relative", false );
    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_SubPasses", false );
    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_Altitude", false );

    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "Checkbox_HotParam", false );
    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "Editbox_HotParam", false );
    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "Button_HotParamUpdate", false );
    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_HotParamName", false );

   
    m_renderer->GUI_RegisterPushButtonEventClickedHandler( m_guiwidgetpushbuttonclicked_cb );
    m_renderer->GUI_RegisterCheckboxEventStateChangedHandler( m_guiwidgetcheckboxstatechanged_cb );

	m_calendar->Startup( 0 );
}

dsreal PlanetViewSubService::compute_arrow_force( void )
{
	return ( 5.0 * std::pow( m_rel_altitude, 1.2 ) );
}

dsreal PlanetViewSubService::compute_arrow_torque( dsreal p_delta )
{
	dsreal torque;

	if( m_rel_altitude < 0.05 )
	{
		torque = 30.0 * abs( p_delta );
	}
	else if( m_rel_altitude < 0.10 )
	{
		torque = 50.0 * abs( p_delta );
	}
	else if( m_rel_altitude < 0.16 )
	{
		torque = 85.0 * abs( p_delta );
	}
	else if( m_rel_altitude < 0.28 )
	{
		torque = 110.0 * abs( p_delta );
	}
	else if( m_rel_altitude < 0.45 )
	{
		torque = 130.0 * abs( p_delta );
	}	
	else if( m_rel_altitude < 3.0 )
	{
		torque = 144.0 * abs( p_delta );
	}
	else if( m_rel_altitude < 4.20 )
	{
		torque = 160.0 * abs( p_delta );
	}
	else
	{
		torque = 200.0 * abs( p_delta );
	}
	return torque;
}


void PlanetViewSubService::Run( void )
{
    /////////////////////////////////////////////////
    if( m_nodes_planetinfos.count( "arrow_body" ) )
    {
        if( m_nodes_planetinfos["arrow_body"]->m_isNodeHot.m_value )
        {
            // set hotstate label visible

            if( !m_renderer->GUI_IsVisible( LAYOUT_FILE, "SimpleLabel_Relative") )
            {
                m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_Relative", true );
                m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_Altitude", true );
            }

            char altitude_text[256];

		    if( m_nodes_planetinfos["arrow_body"]->m_nodeAltitude.m_value > 10000.0 )
		    {
			    sprintf( altitude_text, "Altitude : %.3f km", m_nodes_planetinfos["arrow_body"]->m_nodeAltitude.m_value / 1000.0 );
		    }
		    else
		    {
			    sprintf( altitude_text, "Altitude : %d m", (int)( m_nodes_planetinfos["arrow_body"]->m_nodeAltitude.m_value ) );
		    }

            m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "SimpleLabel_Altitude", altitude_text );
        }
        else
        {
            // set hotstate label invisible
            if( m_renderer->GUI_IsVisible( LAYOUT_FILE, "SimpleLabel_Relative") )
            {
                m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_Relative", false );
                m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_Altitude", false );
            }
        }

        if( m_nodes_planetinfos["arrow_body"]->m_nbSubPasses.m_value > 0 )
        {
            if( !m_renderer->GUI_IsVisible( LAYOUT_FILE, "SimpleLabel_SubPasses" ) )
            {
                m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_SubPasses", true );                
            }
        }
        else
        {
            if( m_renderer->GUI_IsVisible( LAYOUT_FILE, "SimpleLabel_SubPasses" ) )
            {
                m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_SubPasses", false );
            }        
        }
    }

    /////////////////////////////////////////////////

    static dsreal prev_rel_altitude;
    static Matrix prev_arrow_transf;

    if( m_rel_altitude >= 0.0 )
    {
        prev_rel_altitude = m_rel_altitude;
        m_arrow_node->GetFinalTransform( prev_arrow_transf );
    }

    m_scenenodegraph.ComputeTransformations( m_tm );

    m_cdlodp_service->Run();

    if( m_rel_altitude < 0.0 )
    {
        m_arrow->ZeroLSpeed();
        m_arrow->ZeroASpeed();

        m_arrow->ForceInitialAttitude( prev_arrow_transf );
    }

    m_texturepass->GetRenderingQueue()->Draw();
    m_finalpass->GetRenderingQueue()->Draw();

    char renderer_name[64];
    char fps[64];

    sprintf( fps, "%d fps", m_tm.GetFPS() );
    sprintf( renderer_name, "%s", m_pluginDescr.c_str() );

    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_FPS", fps );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_Renderer", renderer_name );

    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_PlanetName", m_planet_conf->m_planetName.m_value );

	if( m_current_camera == m_camera2 )
	{
        if( m_nodes_planetinfos.count( "arrow_body" ) && m_nodes_planetinfos["arrow_body"]->m_nodeRelativeAltitudeValid.m_value )
        {
            m_rel_altitude = m_nodes_planetinfos["arrow_body"]->m_nodeRelativeAltitude.m_value - 1.0;
        }
	}

    char working_set[64];

    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof( PROCESS_MEMORY_COUNTERS ) );

    sprintf( working_set, "%.1f Mb", pmc.WorkingSetSize / 1000000.0 );
    m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Label_Mem", working_set );

    m_renderer->GUI_Render();

    m_renderer->FlipScreen();
	m_calendar->Run();
    
    if( m_tm.IsReady() )
    {
    }
}

void PlanetViewSubService::Release( void )
{
    _DSDEBUG( logger, dsstring("PlanetView sub service : shutdown...") );
}

DrawSpace::Core::BaseSceneNode* PlanetViewSubService::InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler )
{
    return NULL;
}

void PlanetViewSubService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetViewSubService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
}

void PlanetViewSubService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
}

void PlanetViewSubService::OnKeyPress( long p_key )
{
	switch( p_key )
	{
		case VK_SHIFT:

			m_shift = true;
			break;

        case VK_CONTROL:

            m_ctrl = true;
            break;
	}

    m_renderer->GUI_OnKeyDown( p_key );
}

void PlanetViewSubService::OnEndKeyPress( long p_key )
{
	switch( p_key )
	{
		case VK_SHIFT:

			m_shift = false;
			break;

        case VK_CONTROL:

            m_ctrl = false;
            break;
	}

    m_renderer->GUI_OnKeyUp( p_key );
}

void PlanetViewSubService::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:

            m_cdlodp_root->DumpMemoryAllocs();
            break;    
    }
}

void PlanetViewSubService::OnChar( long p_char, long p_scan )
{
    m_renderer->GUI_OnChar( p_char );
}

void PlanetViewSubService::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
	if( m_current_camera == m_camera2 )
	{
		if( m_shift )
		{
			if( m_mouse_left )
			{
				m_objectRot->RotateAxis( Vector( 0.0, 1.0, 0.0, 1.0), p_dx * 2.0, m_tm );
				m_objectRot->RotateAxis( Vector( 1.0, 0.0, 0.0, 1.0), p_dy * 2.0, m_tm );
			}
			else if( m_mouse_right )
			{
				m_objectRot->RotateAxis( Vector( 0.0, 0.0, 1.0, 1.0), -p_dx * 2.0, m_tm );
			}
		}
		else
		{
			if( m_mouse_left )
			{
                if( p_dx > 0 )
                {
                    m_arrow->ApplyLeftYaw( 0.3 * compute_arrow_torque( p_dx ) );
                }
                else if( p_dx < 0 )
                {
                    m_arrow->ApplyRightYaw( 0.3 * compute_arrow_torque( p_dx ) );
                }

                if( p_dy > 0 )
                {
                    m_arrow->ApplyDownPitch( 0.3 * compute_arrow_torque( p_dy ) );
                }
                else if( p_dy < 0 )
                {
                    m_arrow->ApplyUpPitch( 0.3 * compute_arrow_torque( p_dy ) );
                }

			}
			else if( m_mouse_right )
			{
	            if( p_dx < 0 )
	            {
		            m_arrow->ApplyLeftRoll( 1.5 * compute_arrow_torque( p_dx ) );
	            }
	            else if( p_dx > 0 )
	            {
		            m_arrow->ApplyRightRoll( 1.5 * compute_arrow_torque( p_dx ) );
	            }
			}		
		}
	}

    m_renderer->GUI_OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}


void PlanetViewSubService::OnMouseWheel( long p_delta )
{
	if( m_current_camera == m_camera2 )
	{
        dsreal force = DrawSpace::Utils::Maths::Clamp( 20000.0, 5000000.0, 15000000.0 * compute_arrow_force() );

        if( m_ctrl )
        {
		    if( p_delta < 0 )
		    {
			    m_mousewheel_delta++;
		   
		        m_arrow->ApplyRevForce( m_mousewheel_delta * force );
		    }
		    else
		    {
			    m_mousewheel_delta = 0;
                m_arrow->ZeroLSpeed();
		    }         
        }
        else
        {
		    if( p_delta > 0 )
		    {
			    m_mousewheel_delta++;
		   
		        m_arrow->ApplyFwdForce( m_mousewheel_delta * force );
		    }
		    else
		    {
			    m_mousewheel_delta = 0;
                m_arrow->ZeroLSpeed();
		    }        
        }
	}
}

void PlanetViewSubService::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_mouse_left = true;
    m_renderer->GUI_OnMouseLeftButtonDown();
}

void PlanetViewSubService::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_mouse_left = false;
    m_arrow->ZeroASpeed();
    m_renderer->GUI_OnMouseLeftButtonUp();
}

void PlanetViewSubService::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_mouse_right = true;
    m_renderer->GUI_OnMouseRightButtonDown();
}

void PlanetViewSubService::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_mouse_right = false;
    m_arrow->ZeroASpeed();
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
        MainLoopService::GetInstance()->OnGUIEvent( MainLoopService::GUIEVT_PLANETVIEW_CLOSEBUTTON_CLIC );
    }

    if( "Button_ResetPlanetTransform" == p_widget_id )
    {
        m_objectRot->Init( Vector( 0.0, 0.0, 0.0, 1.0 ) );
    }

    if( "Button_ResetAll" == p_widget_id )
    {
        m_objectRot->Init( Vector( 0.0, 0.0, 0.0, 1.0 ) );
        set_arrow_initial_attitude();
    }

    if( "Button_HotParamPrev" == p_widget_id )
    {
        m_hotparams_list_index--;

        if( m_hotparams_list_index == -1 )
        {
            m_hotparams_list_index = m_hotparams_list.size() - 1;
        }
        hotparamslist_index_updated();
    }

    if( "Button_HotParamNext" == p_widget_id )
    {
        m_hotparams_list_index++;

        if( m_hotparams_list_index == m_hotparams_list.size() )
        {
            m_hotparams_list_index = 0;
        }
        hotparamslist_index_updated();
    }

    dsstring text;

    if( "Button_HotParamUpdate" == p_widget_id )
    {
        m_renderer->GUI_GetWidgetText( LAYOUT_FILE, "Editbox_HotParam", text );
        if( "landscapeBumpFactor" == m_hotparams_list[m_hotparams_list_index] )
        {
            try
            {
                m_planet_conf->m_landscapeBumpFactor = (dsreal)std::stoi( text );
            }
            catch( std::invalid_argument )
            {
                dsstring    param_value;
                char        comment[128];

                sprintf( comment, "%d", (int)m_planet_conf->m_landscapeBumpFactor.m_value );
                param_value = comment;
                m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_HotParam", param_value );
            }
        }
        if( "atmoKr" == m_hotparams_list[m_hotparams_list_index] )
        {
            try
            {
                m_planet_conf->m_atmoKr = std::stof( text );
            }
            catch( std::invalid_argument )
            {
                dsstring    param_value;
                char        comment[128];

                sprintf( comment, "%.4f", m_planet_conf->m_atmoKr.m_value );
                param_value = comment;
                m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_HotParam", param_value );
            }        
        }
        if( "groundFogAltLimit" == m_hotparams_list[m_hotparams_list_index] )
        {
            try
            {
                m_planet_conf->m_groundFogAltLimit = (dsreal)std::stoi( text );
            }
            catch( std::invalid_argument )
            {
                dsstring    param_value;
                char        comment[128];

                sprintf( comment, "%d", (int)m_planet_conf->m_groundFogAltLimit.m_value );
                param_value = comment;
                m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_HotParam", param_value );
            }        
        }
        if( "groundFogDensity" == m_hotparams_list[m_hotparams_list_index] )
        {
            try
            {
                m_planet_conf->m_groundFogDensity = std::stof( text );
            }
            catch( std::invalid_argument )
            {
                dsstring    param_value;
                char        comment[128];

                sprintf( comment, "%.5f", (int)m_planet_conf->m_groundFogDensity.m_value );
                param_value = comment;
                m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_HotParam", param_value );
            }        
        }
    }
}

void PlanetViewSubService::on_guicheckboxstatechanged_clicked( const dsstring& p_layout, const dsstring& p_widget_id, bool p_state )
{
    if( p_layout == LAYOUT_FILE && "Checkbox_HotParam" == p_widget_id )
    {
        dsstring hotparam = m_hotparams_list[m_hotparams_list_index];
        if( hotparam == "gravityEnabled" )
        {
            m_planet_conf->m_gravityEnabled = p_state;
        }
        if( hotparam == "atmoRenderEnable" )
        {
            m_planet_conf->m_atmoRenderEnable = p_state;
        }
    }
}

void PlanetViewSubService::Activate( PlanetSceneNodeConfig* p_planetConfig )
{
	m_planet_conf = p_planetConfig;

    m_objectRot->Init( Vector( 0.0, 0.0, 0.0, 1.0 ) );

    ////

    m_renderer->GUI_SetLayout( LAYOUT_FILE );

	create_arrow_camera();
	create_planet( m_planet_conf->m_planetName.m_value );
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();

    //////

    m_hotparams_list_index = 0;
    hotparamslist_index_updated();
}

void PlanetViewSubService::Unactivate( void )
{
	destroy_arrow_camera();
	destroy_planet( m_planet_conf->m_planetName.m_value );
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
}

void PlanetViewSubService::create_passes( void )
{
    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->Initialize();
    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 1, 1, 1, 255 );


    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();

    //m_finalpass->GetRenderingQueue()->EnableTargetClearing( true );
    //m_finalpass->GetRenderingQueue()->SetTargetClearingColor( 255, 255, 255, 255 );

    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    
    RenderStatesSet rss;

    //rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    //rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );

    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    m_finalpass->GetViewportQuad()->GetFx()->SetRenderStates( rss );

    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );

}

void PlanetViewSubService::init_passes( void )
{
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
}

void PlanetViewSubService::create_arrow_camera( void )
{
	m_arrow_params.mass = 10.0;
    m_arrow_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    m_arrow_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 2.0, 2.0, 2.0, 1.0 );
    m_arrow_params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    m_arrow = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Rocket, DrawSpace::Dynamics::Rocket( &m_world, m_arrow_params ) );

    m_arrow->SetExternalMemManagerSource( DrawSpace::Utils::MemAlloc::GetInstance() );
   
    m_arrow_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Rocket>, SceneNode<DrawSpace::Dynamics::Rocket>( "arrow_body" ) );
    m_arrow_node->SetContent( m_arrow );

    m_scenenodegraph.AddNode( m_arrow_node );
    m_scenenodegraph.RegisterNode( m_arrow_node );


    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera2" ) );
    m_camera2_node->SetContent( m_camera2 );

	m_camera2->SetReferentBody( m_arrow );

    m_scenenodegraph.RegisterNode( m_camera2_node );

    m_camera2_node->LinkTo( m_arrow_node );

	set_arrow_initial_attitude();

	m_scenenodegraph.SetCurrentCamera( "camera2" );
	m_current_camera = m_camera2;
}

void PlanetViewSubService::destroy_arrow_camera( void )
{
	m_scenenodegraph.SetCurrentCamera( "" );
	m_current_camera = NULL;

	m_scenenodegraph.UnregisterNode( m_camera2_node );
	m_camera2_node->Unlink();

	m_scenenodegraph.UnregisterNode( m_arrow_node );
	m_scenenodegraph.RemoveNode( m_arrow_node );

	_DRAWSPACE_DELETE_( m_camera2_node );
	_DRAWSPACE_DELETE_( m_camera2 );
	_DRAWSPACE_DELETE_( m_arrow_node );
	_DRAWSPACE_DELETE_( m_arrow );
}


void PlanetViewSubService::create_planet( const dsstring& p_planetId )
{   
    m_planet_node = m_cdlodp_service->InstanciateSceneNode( p_planetId, m_calendar, m_loddepnodeinfosstate_cb );

    m_scenenodegraph.RegisterNode( m_planet_node );

    m_planet_node->LinkTo( m_objectRot_node );
    m_cdlodp_service->RegisterScenegraphCallbacks( m_scenenodegraph );

    m_rel_altitude = 0.0;
}

void PlanetViewSubService::destroy_planet( const dsstring& p_planetId )
{
    m_cdlodp_service->UnregisterScenegraphCallbacks( m_scenenodegraph );

    m_scenenodegraph.UnregisterNode( m_planet_node );   
    m_planet_node->Unlink();

    m_planet_node = NULL;

	m_cdlodp_service->ReleaseSceneNode( p_planetId, m_calendar );

    for( auto it = m_nodes_planetinfos.begin(); it != m_nodes_planetinfos.end(); ++it )
    {
        _DRAWSPACE_DELETE_( it->second );
    }

    m_nodes_planetinfos.clear();
}

void PlanetViewSubService::create_cubes( void )
{
    m_cubescaling = _DRAWSPACE_NEW_( DrawSpace::Core::Transformation, DrawSpace::Core::Transformation );
    m_cubescaling_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::Transformation>, SceneNode<DrawSpace::Core::Transformation>( "cube_scaling" ) );

    m_cubescaling_node->SetContent( m_cubescaling );

    DrawSpace::Utils::Matrix cube_scale;
    cube_scale.Scale( Vector( 500000.0, 500000.0, 500000.0, 1.0 ) );

    m_cubescaling->PushMatrix( cube_scale );

    m_scenenodegraph.AddNode( m_cubescaling_node );
    m_scenenodegraph.RegisterNode( m_cubescaling_node );


    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_chunk->RegisterPassSlot( m_texturepass );


    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    
    m_chunk->GetMeshe()->SetImporter( m_meshe_import );
    m_chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
  
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vso", true ) ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.pso", true ) ) );

    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet rss;

    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "none" ) );

    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->SetRenderStates( rss );
    

    m_chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "chunk" ) );
    m_chunk_node->SetContent( m_chunk );

    
    m_scenenodegraph.RegisterNode( m_chunk_node );


    m_objectRot = _DRAWSPACE_NEW_( DrawSpace::Core::FreeMovement, DrawSpace::Core::FreeMovement );
    m_objectRot_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FreeMovement>, SceneNode<DrawSpace::Core::FreeMovement>( "objectRot" ) );

    m_objectRot->Init( Vector( 0.0, 0.0, 0.0, 1.0 ) );
    m_objectRot->SetSpeed( 0.0 );

    m_objectRot_node->SetContent( m_objectRot );

    m_scenenodegraph.AddNode( m_objectRot_node );
    m_scenenodegraph.RegisterNode( m_objectRot_node );

    m_chunk_node->LinkTo( m_objectRot_node );
}

void PlanetViewSubService::DumpMemoryAllocs( void )
{
    m_cdlodp_root->DumpMemoryAllocs();
}

void PlanetViewSubService::SetCDLODInfos( DrawSpace::Interface::Module::Root* p_cdlodp_root, DrawSpace::Interface::Module::Service* p_cdlodp_service )
{
    m_cdlodp_root = p_cdlodp_root;
    m_cdlodp_service = p_cdlodp_service;

    connect_keys( m_cdlodp_service );
}

void PlanetViewSubService::SetSkyboxInfos( DrawSpace::Interface::Module::Root* p_skyboxp_root, DrawSpace::Interface::Module::Service* p_skyboxp_service )
{
    m_sbmod_root = p_skyboxp_root;
    m_sb_service = p_skyboxp_service;

    connect_keys( m_sb_service );
}

void PlanetViewSubService::set_arrow_initial_attitude( void )
{
	Matrix mat;   
    //mat.Translation( 0.0, 0.0, 4000000.0 );

    mat.Translation( 0.0, 0.0, m_planet_conf->m_planetRay.m_value * 1000 * 9.0 );

	m_arrow->ForceInitialAttitude( mat );
}

void PlanetViewSubService::on_LODdepnodeinfosstate_update( const dsstring& p_nodeid, bool p_state )
{
    if( p_state )
    {
        NodePlanetInfos* pi = _DRAWSPACE_NEW_( NodePlanetInfos, NodePlanetInfos );
        m_cdlodp_service->AddLODDependantNodeInfosKeyLinkTable( m_planet_conf->m_planetName.m_value, p_nodeid, &pi->m_keylinksTable );

        m_nodes_planetinfos[p_nodeid] = pi;
    }
}


void PlanetViewSubService::create_spacebox( void )
{ 
    DrawSpace::Core::BaseSceneNode* spacebox_node = m_sb_service->InstanciateSceneNode( "spacebox", NULL, NULL );

    m_scenenodegraph.RegisterNode( spacebox_node );

    m_spacebox_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "spacebox_transfo" ) );
    m_spacebox_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix spacebox_scale;
    spacebox_scale.Scale( 20.0, 20.0, 20.0 );
    m_spacebox_transfo_node->GetContent()->PushMatrix( spacebox_scale );
    
    m_scenenodegraph.AddNode( m_spacebox_transfo_node );
    m_scenenodegraph.RegisterNode( m_spacebox_transfo_node );

    spacebox_node->LinkTo( m_spacebox_transfo_node );
}

void PlanetViewSubService::hotparamslist_index_updated( void )
{
    dsstring hotparam = m_hotparams_list[m_hotparams_list_index];

    bool cb_display = false;

    bool        cb_state;
    dsstring    param_value;
    char comment[128];
    
    if( hotparam == "gravityEnabled" )
    {
        cb_display = true;
        cb_state = m_planet_conf->m_gravityEnabled.m_value;
    }

    if( hotparam == "atmoRenderEnable" )
    {
        cb_display = true;
        cb_state = m_planet_conf->m_atmoRenderEnable.m_value;
    }

    if( hotparam == "landscapeBumpFactor" )
    {    
        cb_display = false;

        sprintf( comment, "%d", (int)m_planet_conf->m_landscapeBumpFactor.m_value );
        param_value = comment;
    }

    if( hotparam == "atmoKr" )
    {    
        cb_display = false;

        sprintf( comment, "%.4f", m_planet_conf->m_atmoKr.m_value );
        param_value = comment;
    }

    if( hotparam == "groundFogAltLimit" )
    {    
        cb_display = false;

        sprintf( comment, "%d", (int)m_planet_conf->m_groundFogAltLimit.m_value );
        param_value = comment;
    }

    if( hotparam == "groundFogDensity" )
    {    
        cb_display = false;

        sprintf( comment, "%.5f", m_planet_conf->m_groundFogDensity.m_value );
        param_value = comment;
    }
    //////////////////////////////////////////////////////////////////////////////////////////

    if( cb_display )
    {
        m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Checkbox_HotParam", hotparam );
        m_renderer->GUI_SetCheckboxState( LAYOUT_FILE, "Checkbox_HotParam", cb_state );
    }
    else
    {
        m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "SimpleLabel_HotParamName", hotparam );
        m_renderer->GUI_SetWidgetText( LAYOUT_FILE, "Editbox_HotParam", param_value );
    }

    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "SimpleLabel_HotParamName", !cb_display );
    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "Editbox_HotParam", !cb_display );
    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "Button_HotParamUpdate", !cb_display );
    m_renderer->GUI_SetVisibleState( LAYOUT_FILE, "Checkbox_HotParam", cb_display );
}
