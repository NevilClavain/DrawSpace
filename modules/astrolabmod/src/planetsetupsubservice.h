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

#ifndef _PLANETSETUPSUBSERVICE_H_
#define _PLANETSETUPSUBSERVICE_H_

#include "drawspace.h"
#include "crtp_singleton.h"
#include "planetscenenodeconfig.h"

class MainLoopService;

class PlanetSetupSubService : public DrawSpace::Interface::Module::Service, public BaseSingleton<PlanetSetupSubService>
{
protected:

    typedef DrawSpace::Core::CallBack2<PlanetSetupSubService, void, const dsstring&, const dsstring&>   GUIWidgetPushButtonClickedCallback;

    typedef DrawSpace::Core::CallBack<PlanetSetupSubService, void, DrawSpace::Utils::Timer*>            TimerCb;


    DrawSpace::Interface::Renderer*                                     m_renderer;
    DrawSpace::Utils::TimeManager                                       m_tm;
    dsstring                                                            m_pluginDescr;

    GUIWidgetPushButtonClickedCallback*                                 m_guiwidgetpushbuttonclicked_cb;
    DrawSpace::Core::BaseCallback<void, int>*                           m_closeapp_cb;

    std::map<dsstring, PlanetSceneNodeConfig>							m_nodes_config;

    TimerCb*                                                            m_statusbar_timer_cb;
    DrawSpace::Utils::Timer*                                            m_statusbar_timer;


    DrawSpace::Interface::Module::Root*                                 m_cdlodp_root;
    DrawSpace::Interface::Module::Service*                              m_cdlodp_service;

	PlanetSceneNodeConfig*												m_selected_planet_conf;

    void on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id );

    void on_statusbar_timer( DrawSpace::Utils::Timer* p_timer );
    void statusbar_msg( const dsstring& p_msg );

    void update_listbox( void );

    PlanetSetupSubService( void );
public:
    ~PlanetSetupSubService( void );


    virtual void                            GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys );
    virtual void                            Init( DrawSpace::Logger::Configuration* p_logconf, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb );
    virtual void                            Run( void );
    virtual void                            Release( void );

    virtual DrawSpace::Core::BaseSceneNode* InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler );
    virtual void                            RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );
    virtual void                            UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );
    virtual void                            ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar );


    virtual void                            OnKeyPress( long p_key );
    virtual void                            OnEndKeyPress( long p_key );
    virtual void                            OnKeyPulse( long p_key );
    virtual void                            OnChar( long p_char, long p_scan );
    virtual void                            OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy );
    virtual void                            OnMouseWheel( long p_delta );
    virtual void                            OnMouseLeftButtonDown( long p_xm, long p_ym );
    virtual void                            OnMouseLeftButtonUp( long p_xm, long p_ym );
    virtual void                            OnMouseRightButtonDown( long p_xm, long p_ym );
    virtual void                            OnMouseRightButtonUp( long p_xm, long p_ym );
    virtual void                            OnAppEvent( WPARAM p_wParam, LPARAM p_lParam );

    virtual void                            Activate( void );
    virtual void                            Unactivate( void );

    virtual void                            SetCDLODInfos( DrawSpace::Interface::Module::Root* p_cdlodp_root, DrawSpace::Interface::Module::Service* p_cdlodp_service );

	//virtual dsstring						GetSelectedPlanetId(void);
	virtual PlanetSceneNodeConfig*			GetSelectedPlanetConfig( void );

    friend class BaseSingleton<PlanetSetupSubService>;

};

#endif