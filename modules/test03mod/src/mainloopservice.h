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
#include "timeaspect.h"

class MainLoopService : public DrawSpace::Interface::Module::Service
{
protected:

    typedef DrawSpace::Core::CallBack2<MainLoopService, void, DrawSpace::Systems::TransformSystem::Event, DrawSpace::Core::Entity*>             CameraEventHandler;
    typedef DrawSpace::Core::CallBack2<MainLoopService, void, DrawSpace::EntityGraph::EntityNode::Event, DrawSpace::Core::Entity*>          EntitygraphNodeEventHandler;

    bool                                                                            m_left_mousebutton;
    bool                                                                            m_right_mousebutton;

    DrawSpace::Interface::Renderer*                                                 m_renderer;
    DrawSpace::Utils::TimeManager                                                   m_tm;
    dsstring                                                                        m_pluginDescr;

    DrawSpace::RenderGraph::RenderPassNodeGraph                                     m_rendergraph;
    DrawSpace::EntityGraph::EntityNodeGraph                                         m_entitygraph;

    DrawSpace::RenderGraph::RenderPassNode                                          m_texturepass;
    DrawSpace::RenderGraph::RenderPassNode                                          m_finalpass;

    DrawSpace::Core::Entity                                                         m_rootEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_rootEntityNode;

    DrawSpace::Core::Entity                                                         m_timeEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_timeEntityNode;

    DrawSpace::Core::Entity                                                         m_camerasTimeEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_camerasTimeEntityNode;


    DrawSpace::Core::Entity                                                         m_world1Entity;
    DrawSpace::EntityGraph::EntityNode                                              m_World1EntityNode;

    DrawSpace::Core::Entity                                                         m_world2Entity;
    DrawSpace::EntityGraph::EntityNode                                              m_World2EntityNode;

    DrawSpace::Core::Entity                                                         m_cubeEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_cubeEntityNode;

    DrawSpace::Core::Entity                                                         m_sphereEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_sphereEntityNode;

    DrawSpace::Core::Entity                                                         m_groundEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_groundEntityNode;

    DrawSpace::Core::Entity                                                         m_skyboxEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_skyboxEntityNode;


    DrawSpace::Core::Entity                                                         m_cameraEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_cameraEntityNode;

    DrawSpace::Core::Entity                                                         m_camera2Entity;
    DrawSpace::EntityGraph::EntityNode                                              m_camera2EntityNode;



    DrawSpace::Core::Entity                                                         m_centralSphereEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_centralSphereEntityNode;




    DrawSpace::Systems::RenderingSystem                                             m_renderingSystem;
    DrawSpace::Systems::TransformSystem                                             m_transformSystem;
    DrawSpace::Systems::PhysicsSystem                                               m_physicsSystem;
    DrawSpace::Systems::TimeSystem                                                  m_timeSystem;


    DrawSpace::AspectImplementations::PassesRenderingAspectImpl                     m_passesRender;
    DrawSpace::AspectImplementations::MesheRenderingAspectImpl                      m_cubeRender;
    DrawSpace::AspectImplementations::MesheRenderingAspectImpl                      m_sphereRender;
    DrawSpace::AspectImplementations::MesheRenderingAspectImpl                      m_centralSphereRender;
    DrawSpace::AspectImplementations::MesheRenderingAspectImpl                      m_groundRender;
    DrawSpace::AspectImplementations::TextRenderingAspectImpl                       m_textRender;
    DrawSpace::AspectImplementations::SkyboxRenderingAspectImpl                     m_skyboxRender;

    DrawSpace::Aspect::TimeAspect::TimeAngle                                        m_planet_rot;

    DrawSpace::Aspect::TimeAspect::TimeAngle                                        m_fps_yaw;
    DrawSpace::Aspect::TimeAspect::TimeAngle                                        m_fps_pitch;



    DrawSpace::AspectImplementations::RawTransformAspectImpl                        m_transformer;
    DrawSpace::AspectImplementations::FPSTransformAspectImpl                        m_fps_transformer;
    DrawSpace::AspectImplementations::FreeTransformAspectImpl                       m_free_transformer;
    
    DrawSpace::Interface::MesheImport*                                              m_meshe_import;

    int                                                                             m_current_camera;

    CameraEventHandler                                                              m_worldsystem_evt_handler;
    EntitygraphNodeEventHandler                                                     m_entitygraph_evt_handler;

    bool                                                                            m_show_cube;

    bool                                                                            m_cube_is_relative;


    void on_transformsystem_evt( DrawSpace::Systems::TransformSystem::Event p_evt, DrawSpace::Core::Entity* p_entity );

    void on_entitygraph_evt( DrawSpace::EntityGraph::EntityNode::Event p_evt, DrawSpace::Core::Entity* p_entity );


    void create_cube( const DrawSpace::Utils::Matrix& p_transform, DrawSpace::Core::Entity& p_entity );
    void create_skybox( void );
    void create_ground( void );

    void create_sphere( const DrawSpace::Utils::Matrix& p_transform, DrawSpace::Core::Entity& p_entity, DrawSpace::AspectImplementations::MesheRenderingAspectImpl* p_render, DrawSpace::Aspect::BodyAspect::Mode p_mode );

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
