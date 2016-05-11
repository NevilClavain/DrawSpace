/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _PLANETOID_BODY_H_
#define _PLANETOID_BODY_H_

#include "spherelod_subpass.h"
#include "spherelod_layer.h"
#include "scenenodegraph.h"
#include "noise.h"

namespace DrawSpace
{
namespace Planetoid
{

class Body : public Dynamics::Orbiter
{
public:

    typedef DrawSpace::Core::CallBack2<DrawSpace::Planetoid::Body, void, DrawSpace::Core::SceneNodeGraph::CameraEvent, DrawSpace::Core::BaseSceneNode*>             CameraEvtCb;
    typedef DrawSpace::Core::CallBack2<DrawSpace::Planetoid::Body, void, DrawSpace::Core::SceneNodeGraph::NodesEvent, DrawSpace::Core::BaseSceneNode*>              NodesEventCb;
    typedef DrawSpace::Core::CallBack2<DrawSpace::Planetoid::Body, void, DrawSpace::Core::SceneNodeGraph::ScenegraphEvent, DrawSpace::Core::SceneNodeGraph*>        ScenegraphEventCb;
    typedef DrawSpace::Core::CallBack2<DrawSpace::Planetoid::Body, DrawSpace::SphericalLOD::SubPass::EntryInfos, DrawSpace::SphericalLOD::SubPass*, int>            SubPassCreationCb;

    typedef DrawSpace::Core::CallBack<DrawSpace::Planetoid::Body, void, DrawSpace::Utils::Timer*>                                                                   TimerCb;

protected:

    typedef struct
    {
        bool                                        attached;

        bool                                        relative_alt_valid;
        dsreal                                      relative_alt;

        DrawSpace::Dynamics::InertBody*             body;

        std::vector<SphericalLOD::Layer*>           layers;

        DrawSpace::IntermediatePass*                collidingheightmap_pass;
        DrawSpace::SphericalLOD::FaceDrawingNode*   collidingheightmap_node;
        
    } RegisteredBody;


    typedef enum
    {
        FREE,
        FREE_ON_PLANET,
        INERTBODY_LINKED,

    } CameraType;

    typedef struct
    {
        bool                                update_meshe;

        CameraType                          type;
        DrawSpace::Dynamics::InertBody*     attached_body;
        DrawSpace::Dynamics::Collider*      attached_collider;

        DrawSpace::Dynamics::CameraPoint*   camera;

        std::vector<SphericalLOD::Layer*>   layers;

    } RegisteredCamera;


    dsreal                                                                  m_ray;

    DrawSpace::Dynamics::World                                              m_world;
    dsstring                                                                m_scenename;
        
    DrawSpace::SphericalLOD::Drawing*                                       m_drawable;
    
    CameraEvtCb*                                                            m_camera_evt_cb;
    NodesEventCb*                                                           m_nodes_evt_cb;
    ScenegraphEventCb*                                                      m_scenegraph_evt_cb;    
    SubPassCreationCb*                                                      m_subpass_creation_cb;
    TimerCb*                                                                m_timer_cb;
      
    std::map<DrawSpace::Dynamics::InertBody*, RegisteredBody>               m_registered_bodies;
    std::map<dsstring, RegisteredCamera>                                    m_registered_camerapoints;
    dsstring                                                                m_current_camerapoint;

    std::vector<SphericalLOD::Layer*>                                       m_layers_list;

    DrawSpace::Utils::TimeManager*                                          m_timemanager;
    DrawSpace::Utils::Timer*                                                m_timer;

    DrawSpace::SphericalLOD::Config*                                        m_config;

    // lists of some passes to render for internal stuff
    // colliding heightmap, global textures ?

    SphericalLOD::SubPass::singleshot_subpasses_stack                       m_singleshot_subpasses_stack;
    SphericalLOD::SubPass::singleshot_subpasses                             m_singleshot_subpasses;
    SphericalLOD::SubPass::permanent_subpasses                              m_permanent_subpasses;


    void attach_body( DrawSpace::Dynamics::InertBody* p_body );
    void detach_body( DrawSpace::Dynamics::InertBody* p_body );
    void body_find_attached_camera( DrawSpace::Dynamics::InertBody* p_body, std::vector<dsstring>& p_name );

    void on_camera_event( DrawSpace::Core::SceneNodeGraph::CameraEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );
    void on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );
    void on_scenegraph_event( DrawSpace::Core::SceneNodeGraph::ScenegraphEvent p_event, DrawSpace::Core::SceneNodeGraph* p_scenegraph );
    
    DrawSpace::SphericalLOD::SubPass::EntryInfos on_subpasscreation( DrawSpace::SphericalLOD::SubPass* p_pass, int p_dest );
    void on_timer( DrawSpace::Utils::Timer* p_timer );

    void create_camera_collisions( const dsstring& p_cameraname, DrawSpace::Dynamics::CameraPoint* p_camera, RegisteredCamera& p_cameradescr, bool p_hotstate );

    void apply_gravity( void );
    void manage_bodies( void );
    void manage_camerapoints( void );
    void compute_layers( void );
    void update_cameras_alt( void );

public:
    
    Body( const dsstring& p_scenename, dsreal p_ray, DrawSpace::Utils::TimeManager* p_time, const DrawSpace::SphericalLOD::Config& p_config );
    virtual ~Body( void );

    void                                GetSceneName( dsstring& p_name );    
    //void                                GetCameraHotpoint( const dsstring& p_name, DrawSpace::Utils::Matrix& p_outmat );

    SphericalLOD::FaceDrawingNode*      RegisterSinglePlanetBodyPassSlot( Pass* p_pass, SphericalLOD::Binder* p_binder, int p_orientation, 
                                                                            DrawSpace::SphericalLOD::Body::MesheType p_meshe_type, int p_layer_index );

    SphericalLOD::Layer*                GetLayer( DrawSpace::Dynamics::InertBody* p_body, int p_layer_index );
    
    void                                RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );

    void                                SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );
    void                                OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );
    void                                Update( DrawSpace::Utils::TimeManager& p_timemanager );
    void                                Update2( DrawSpace::Utils::TimeManager& p_timemanager );

    bool                                GetInertBodyRelativeAltitude( DrawSpace::Dynamics::InertBody* p_body, dsreal& p_rel_altitude );

    void                                DrawSubPasses( void );


    void                                ResetRegisteredBodyLayer( DrawSpace::Dynamics::InertBody* p_body, int p_layer_index );

    int                                 GetSingleShotSubPassesStackSize();
};



}
}


#endif
