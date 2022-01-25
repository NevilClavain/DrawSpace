/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#pragma once

#include "drawspace.h"
#include "serviceaspectimpl.h"
#include "fpstransformaspectimpl.h"
#include "impostorsrenderingaspectimpl.h"
#include "meshrenderingaspectimpl.h"
#include "stringrenderingaspectimpl.h"


class MainService : public DrawSpace::Interface::AspectImplementations::ServiceAspectImpl
{
protected:

    using ResourceEventCallback = DrawSpace::Core::CallBack3<MainService, void, DrawSpace::Systems::ResourcesSystem::ResourceEvent, const dsstring&, const dsstring&>;

    DrawSpace::Interface::Renderer*                                                 m_renderer;
    dsstring                                                                        m_pluginDescr;

    DrawSpace::Interface::MesheImport*                                              m_meshe_import;

    DrawSpace::RenderGraph::RenderPassNodeGraph                                     m_rendergraph;
    DrawSpace::EntityGraph::EntityNodeGraph                                         m_entitygraph;

    DrawSpace::RenderGraph::RenderPassNode                                          m_texturepass;
    DrawSpace::RenderGraph::RenderPassNode                                          m_finalpass;    



    DrawSpace::Core::Entity                                                         m_rootEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_rootEntityNode;

    DrawSpace::Core::Entity                                                         m_world1Entity;
    DrawSpace::EntityGraph::EntityNode                                              m_World1EntityNode;


    DrawSpace::Core::Entity                                                         m_cameraEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_cameraEntityNode;

    DrawSpace::Core::Entity                                                         m_groundEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_groundEntityNode;

    DrawSpace::Core::Entity                                                         m_cubeEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_cubeEntityNode;

    DrawSpace::Core::Entity                                                         m_cube2Entity;
    DrawSpace::EntityGraph::EntityNode                                              m_cube2EntityNode;

    DrawSpace::Core::Entity                                                         m_mainBodyEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_mainBodyEntityNode;

    DrawSpace::Core::Entity                                                         m_feetEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_feetEntityNode;



    DrawSpace::Core::Entity                                                         m_impostorsEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_impostorsEntityNode;

    DrawSpace::Core::Entity                                                         m_worldImpostorsEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_worldImpostorsEntityNode;

    DrawSpace::Core::Entity                                                         m_spriteEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_spriteEntityNode;

    DrawSpace::Core::Entity                                                         m_spriteCollimatorEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_spriteCollimatorEntityNode;


    DrawSpace::Core::Entity                                                         m_skyboxEntity;
    DrawSpace::EntityGraph::EntityNode                                              m_skyboxEntityNode;


    DrawSpace::Systems::Hub                                                         m_systemsHub;

    DrawSpace::AspectImplementations::PassesRenderingAspectImpl                     m_passesRender;
    DrawSpace::Interface::AspectImplementations::RenderingAspectImpl*               m_skyboxRender;
    MeshRenderingAspectImpl                                                         m_groundRender;
    MeshRenderingAspectImpl                                                         m_cubeRender;
    MeshRenderingAspectImpl                                                         m_cube2Render;
    MeshRenderingAspectImpl                                                         m_mainBodyRender;
    MeshRenderingAspectImpl                                                         m_feetRender;

    ImpostorsRenderingAspectImpl                                                    m_impostorsRender;
    ImpostorsRenderingAspectImpl                                                    m_worldImpostorsRender;
    ImpostorsRenderingAspectImpl                                                    m_spriteRender;
    ImpostorsRenderingAspectImpl                                                    m_spriteCollimatorRender;

    DrawSpace::Core::RenderingNode*                                                 m_sprite_texturepass_rnode;

    DrawSpace::Core::RenderingNode*                                                 m_spritecollimator_texturepass_rnode;

    StringRenderingAspectImpl                                                       m_textRender;


    StringRenderingAspectImpl                                                       m_entityIdTextRender;

    StringRenderingAspectImpl                                                       m_entityDistanceTextRender;
    dsreal                                                                          m_entityDistance = 0.0;


    DrawSpace::AspectImplementations::RawTransformAspectImpl                        m_skybox_transformer;
    DrawSpace::AspectImplementations::RawTransformAspectImpl                        m_impostors_transformer;
    DrawSpace::AspectImplementations::RawTransformAspectImpl                        m_composition_transformer;
    DrawSpace::AspectImplementations::RawTransformAspectImpl                        m_ground_transformer;
    DrawSpace::AspectImplementations::RigidBodyTransformAspectImpl                  m_rigbody_cube_transformer;
    DrawSpace::AspectImplementations::RigidBodyTransformAspectImpl                  m_rigbody_cube_transformer2;

    DrawSpace::AspectImplementations::RigidBodyTransformAspectImpl                  m_rigbody_composition_transformer;
    FPSTransformAspectImpl                                                          m_fps_transformer;


    DrawSpace::Core::BaseCallback<void, bool>*                                      m_mousecircularmode_cb;
    DrawSpace::Core::BaseCallback<void, int>*                                       m_closeapp_cb;



    DrawSpace::Aspect::TimeAspect::TimeAngle                                        m_fps_yaw;
    DrawSpace::Aspect::TimeAspect::TimeAngle                                        m_fps_pitch;

    ResourceEventCallback                                                           m_resource_events_cb;


    void set_mouse_circular_mode( bool p_state );

    void create_skybox( void );
    void create_ground( void );
    void create_cube( dsreal p_x, dsreal p_y, dsreal p_z, MeshRenderingAspectImpl& p_rendering_aspect_impl, DrawSpace::Core::Entity& p_entity,
        DrawSpace::AspectImplementations::RigidBodyTransformAspectImpl& p_rigidBodyTransformAspectImpl);


    void create_composition(dsreal p_x, dsreal p_y, dsreal p_z, 
                            MeshRenderingAspectImpl& p_rendering_aspect_impl, 
                            DrawSpace::Core::Entity& p_entity,
                                DrawSpace::AspectImplementations::RawTransformAspectImpl& p_transform_impl,
                                MeshRenderingAspectImpl& p_rendering_aspect_impl_2,
                                DrawSpace::Core::Entity& p_entity_2,
                                DrawSpace::AspectImplementations::RigidBodyTransformAspectImpl& p_rigidBodyTransformAspectImpl
                                );


    void create_camera( void );
    void create_screen_impostors( void );
    void create_world_impostor( void );
    void create_sprite_impostor(void);
    void create_collimator_sprite_impostor(void);

    void on_resource_event(DrawSpace::Systems::ResourcesSystem::ResourceEvent p_event, const dsstring& p_resource, const dsstring& p_context);


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
