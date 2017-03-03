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

#ifndef _PLANETVIEWSUBSERVICE_H_
#define _PLANETVIEWSUBSERVICE_H_

#include "drawspace.h"
#include "crtp_singleton.h"
#include "planetscenenodeconfig.h"

class MainLoopService;

class PlanetViewSubService : public DrawSpace::Interface::Module::Service, public BaseSingleton<PlanetViewSubService>
{
protected:

    typedef DrawSpace::Core::CallBack2<PlanetViewSubService, void, const dsstring&, const dsstring&>  GUIWidgetPushButtonClickedCallback;

    DrawSpace::Interface::Renderer*                                     m_renderer;
    DrawSpace::Utils::TimeManager                                       m_tm;
    dsstring                                                            m_pluginDescr;

	DrawSpace::Dynamics::World											m_world;
	DrawSpace::Dynamics::Calendar*										m_calendar;

    DrawSpace::Interface::MesheImport*                                  m_meshe_import;

    GUIWidgetPushButtonClickedCallback*                                 m_guiwidgetpushbuttonclicked_cb;
    DrawSpace::Core::BaseCallback<void, int>*                           m_closeapp_cb;

    DrawSpace::FinalPass*                                               m_finalpass;
    DrawSpace::IntermediatePass*                                        m_texturepass;

	int																	m_mousewheel_delta;

    DrawSpace::Core::SceneNodeGraph                                     m_scenenodegraph;



    DrawSpace::Dynamics::CameraPoint*                                   m_camera2;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera2_node;
    DrawSpace::Dynamics::Rocket*										m_arrow;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Rocket>*			m_arrow_node;
	DrawSpace::Dynamics::Body::Parameters								m_arrow_params;



    DrawSpace::Core::FreeMovement*                                      m_objectRot;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FreeMovement>*          m_objectRot_node;

    DrawSpace::Chunk*                                                   m_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_chunk_node;

    DrawSpace::Core::Transformation*                                    m_cubescaling;
    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_cubescaling_node;


    DrawSpace::Core::BaseSceneNode*                                     m_planet_node;

    bool                                                                m_mouse_left;
    bool                                                                m_mouse_right;

	double																m_rel_altitude;

    DrawSpace::Interface::Module::Root*                                 m_cdlodp_root;
    DrawSpace::Interface::Module::Service*                              m_cdlodp_service;

    DrawSpace::Module::KeySource<DrawSpace::Core::SceneNodeGraph*>      m_cdlodplanet_scenenodegraph;
    DrawSpace::Module::KeySource<DrawSpace::IntermediatePass*>          m_cdlodplanet_texturepass;

	PlanetSceneNodeConfig*												m_planet_conf;

	DrawSpace::Dynamics::CameraPoint*									m_current_camera;

	bool																m_shift;
    bool                                                                m_ctrl;


    void init_passes( void );
    void create_passes( void );

    void create_cubes( void );

	void create_arrow_camera( void );
	void destroy_arrow_camera( void );

    void create_planet( const dsstring& p_planetId );
	void destroy_planet(const dsstring& p_planetId);

    void on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id );

	dsreal compute_arrow_force( void );
	dsreal compute_arrow_torque( dsreal p_delta );

	void set_arrow_initial_attitude( void );

    PlanetViewSubService( void );
public:
    ~PlanetViewSubService( void );


    virtual void                            GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys );
    virtual void                            Init( DrawSpace::Logger::Configuration* p_logconf, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb );
    virtual void                            Run( void );
    virtual void                            Release( void );

    virtual DrawSpace::Core::BaseSceneNode* InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar );
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


	virtual void                            Activate( PlanetSceneNodeConfig* p_planetConfig );
    virtual void                            Unactivate( void );

    virtual void                            DumpMemoryAllocs( void );

    virtual void                            SetCDLODInfos( DrawSpace::Interface::Module::Root* p_cdlodp_root, DrawSpace::Interface::Module::Service* p_cdlodp_service );

    friend class BaseSingleton<PlanetViewSubService>;
};

#endif