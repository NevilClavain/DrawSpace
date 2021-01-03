/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#ifndef _MAINSERVICE_H_
#define _MAINSERVICE_H_

#include "drawspace.h"
#include "serviceaspectimpl.h"

class MainService : public DrawSpace::Interface::AspectImplementations::ServiceAspectImpl
{
protected:

    typedef DrawSpace::Core::CallBack2<MainService, void, const dsstring&, const dsstring&>                     GUIWidgetPushButtonClickedCallback;
    typedef DrawSpace::Core::CallBack<MainService, void, DrawSpace::Systems::Hub::SystemsUpdateEvent>           SystemsUpdateEvtCb;

    DrawSpace::Interface::Renderer*                                                 m_renderer;
    dsstring                                                                        m_pluginDescr;

    DrawSpace::Interface::MesheImport*                                              m_meshe_import;

    DrawSpace::RenderGraph::RenderPassNodeGraph                                     m_rendergraph;
    DrawSpace::EntityGraph::EntityNodeGraph                                         m_entitygraph;

    DrawSpace::RenderGraph::RenderPassNode                                          m_texturepass;
    DrawSpace::RenderGraph::RenderPassNode                                          m_texturemirrorpass;
    DrawSpace::RenderGraph::RenderPassNode                                          m_wavespass;
    DrawSpace::RenderGraph::RenderPassNode                                          m_bumppass;
    DrawSpace::RenderGraph::RenderPassNode                                          m_finalpass;    

    DrawSpace::Core::Entity                                                         m_rootEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_rootEntityNode;

    DrawSpace::Core::Entity                                                         m_skyboxEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_skyboxEntityNode;

    DrawSpace::Core::Entity                                                         m_groundEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_groundEntityNode;

    DrawSpace::Core::Entity                                                         m_staticCubeEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_staticCubeEntityNode;


    DrawSpace::Core::Entity                                                         m_world1Entity;
    DrawSpace::EntityGraph::EntityNode                                              m_World1EntityNode;


    DrawSpace::Systems::Hub                                                         m_systemsHub;

    DrawSpace::AspectImplementations::PassesRenderingAspectImpl                     m_passesRender;
    DrawSpace::Interface::AspectImplementations::RenderingAspectImpl*               m_skyboxRender;
    DrawSpace::AspectImplementations::MesheRenderingAspectImpl                      m_groundRender;
    DrawSpace::AspectImplementations::MesheRenderingAspectImpl                      m_staticCubeRender;
    DrawSpace::AspectImplementations::TextRenderingAspectImpl                       m_textRender;

    DrawSpace::AspectImplementations::RawTransformAspectImpl                        m_skybox_transformer;                       
    DrawSpace::Interface::AspectImplementations::TransformAspectImpl*               m_fps_transformer;

    DrawSpace::Core::Entity                                                         m_cameraEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_cameraEntityNode;


    DrawSpace::Aspect::TimeAspect::TimeAngle                                        m_fps_yaw;
    DrawSpace::Aspect::TimeAspect::TimeAngle                                        m_fps_pitch;

    DrawSpace::Aspect::TimeAspect::TimeScalar                                       m_waves;
    bool                                                                            m_waves_inc;

    bool                                                                            m_hmi_mode;

    DrawSpace::Core::BaseCallback<void, bool>*                                      m_mousecircularmode_cb;
    DrawSpace::Core::BaseCallback<void, int>*                                       m_closeapp_cb;

    GUIWidgetPushButtonClickedCallback                                              m_guiwidgetpushbuttonclicked_cb;

    typedef struct
    {
        DrawSpace::Core::Entity*                                                    dynCubeEntity;
        DrawSpace::EntityGraph::EntityNode*                                         dynCubeEntityNode;
        DrawSpace::AspectImplementations::MesheRenderingAspectImpl*                 dynCubeRender;
        DrawSpace::Aspect::BodyAspect*                                              dynCubeBodyAspect;

    } dynamic_cube;

    std::vector<dynamic_cube>                                                       m_dynamic_cubes;

    SystemsUpdateEvtCb                                                              m_systems_update_evt_cb;

    void set_mouse_circular_mode( bool p_state );

    void on_guipushbutton_clicked( const dsstring& p_layout, const dsstring& p_widget_id );
    void on_systems_update_evt( DrawSpace::Systems::Hub::SystemsUpdateEvent p_evt );

    void create_skybox( void );
    void create_ground( void );
    void create_static_cube( void );
    void create_dynamic_cube( void );

    void clean_cubes( void );

public:

    MainService( void );

    bool Init( void );
    void Run( void );
    void Release( void );

    void OnKeyPress( long p_key );
    void OnEndKeyPress( long p_key );
    void OnKeyPulse( long p_key );
    void OnChar( long p_char, long p_scan );
    void OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy );
    void OnMouseWheel( long p_delta );
    void OnMouseLeftButtonDown( long p_xm, long p_ym );
    void OnMouseLeftButtonUp( long p_xm, long p_ym );
    void OnMouseRightButtonDown( long p_xm, long p_ym );
    void OnMouseRightButtonUp( long p_xm, long p_ym );
    void OnAppEvent( WPARAM p_wParam, LPARAM p_lParam );

};

#endif
