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

#ifndef _MAINLOOPSERVICE_H_
#define _MAINLOOPSERVICE_H_

#include "module_service.h"
#include "drawspace.h"

class MainLoopService : public DrawSpace::Interface::Module::Service
{
protected:

    typedef struct
    {
        DrawSpace::Chunk*                                                   chunk;
        DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       chunk_node;
        DrawSpace::Dynamics::InertBody*                                     cube_body;
        DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*         cube_body_node;
        
    } CUBE_ENTRY;

    typedef DrawSpace::Core::CallBack2<MainLoopService, void, const dsstring&, const dsstring&>  GUIWidgetPushButtonClickedCallback;

    DrawSpace::Interface::Renderer*                                     m_renderer;
    DrawSpace::Utils::TimeManager                                       m_tm;
    dsstring                                                            m_pluginDescr;

    DrawSpace::Interface::MesheImport*                                  m_meshe_import;

    DrawSpace::Dynamics::World                                          m_world;

    DrawSpace::FinalPass*                                               m_finalpass;
    DrawSpace::IntermediatePass*                                        m_texturepass;
    DrawSpace::IntermediatePass*                                        m_texturemirrorpass;
    DrawSpace::IntermediatePass*                                        m_wavespass;
    DrawSpace::IntermediatePass*                                        m_bumppass;

    DrawSpace::IntermediatePass*                                        m_coloredpass;

    DrawSpace::Interface::Module::Root*                                 m_sbmod_root;
    DrawSpace::Interface::Module::Service*                              m_sb_service;


    
    /*
    DrawSpace::Chunk*                                                   m_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_chunk_node;
    DrawSpace::Dynamics::InertBody*                                     m_cube_body;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*         m_cube_body_node;
    */

    std::vector<CUBE_ENTRY>                                             m_cubes;



    DrawSpace::Chunk*                                                   m_cube2;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_cube2_node;
    DrawSpace::Dynamics::Collider*                                      m_cube2_collider;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Collider>*          m_cube2_colider_node;
    DrawSpace::Core::FreeMovement                                       m_freemove;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FreeMovement>*          m_freemove_node;


    DrawSpace::Dynamics::InertBody*                                     m_ground_body;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*         m_ground_body_node;

    DrawSpace::Chunk*                                                   m_ground;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_ground_node;

    dsreal                                                              m_waves;
    bool                                                                m_waves_inc;



    DrawSpace::Dynamics::CameraPoint*                                   m_camera;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera_node;

    DrawSpace::Core::FPSMovement                                        m_fpsmove;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FPSMovement>*           m_fpsmove_node;

    DrawSpace::Core::SceneNodeGraph                                     m_scenenodegraph;

    DrawSpace::Core::BaseCallback<void, bool>*                          m_mousecircularmode_cb;
    DrawSpace::Core::BaseCallback<void, int>*                           m_closeapp_cb;


    void*                                                               m_texturecontent;

    DrawSpace::Core::Texture*                                           m_texcube2;
    void*                                                               m_tc2content;

    bool                                                                m_hmi_mode;

    GUIWidgetPushButtonClickedCallback*                                 m_guiwidgetpushbuttonclicked_cb;


    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_spacebox_transfo_node;

    /////////////////////////////////////////////////////////////////////////////////

    DrawSpace::Module::KeySource<DrawSpace::IntermediatePass*>          m_skybox_texturepass;
    DrawSpace::Module::KeySource<DrawSpace::IntermediatePass*>          m_skybox_texturemirrorpass;
    DrawSpace::Module::KeySource<DrawSpace::Utils::Vector>              m_skybox_reflectornormale;
    DrawSpace::Module::KeySource<dsstring>                              m_skybox_texturesbankpath;
    DrawSpace::Module::KeySource<dsstring>                              m_skybox_texturesbankvirtualfspath;
    DrawSpace::Module::KeySource<std::vector<dsstring>>                 m_skybox_texturesnames;

    /////////////////////////////////////////////////////////////////////////////////


    void    create_passes( void );
    void    init_passes( void );

    void    load_skybox_module( void );

    void    create_spacebox( void );
    void    create_camera( void );
    void    create_static_cube( void );

    void    create_dynamic_cube( void );

    void    create_ground( void );

    void    set_mouse_circular_mode( bool p_state );

    void    on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id );

public:
    MainLoopService( void );
    ~MainLoopService( void );

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
};

#endif
