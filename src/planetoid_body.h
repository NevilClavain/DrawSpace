/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "planetoid_fragment.h"
#include "scenenodegraph.h"
#include "noise.h"

namespace DrawSpace
{
namespace Planetoid
{

class Body : public Dynamics::Orbiter
{
public:

    ////////////////////////////////////////////////////////////////////
    bool                    m_front_done;
    bool                    m_rear_done;
    bool                    m_left_done;
    bool                    m_right_done;
    bool                    m_top_done;
    bool                    m_bottom_done;

    void                    run_textures( DrawSpace::Pass* p_pass );

    ////////////////////////////////////////////////////////////////////


    typedef DrawSpace::Core::CallBack2<DrawSpace::Planetoid::Body, void, DrawSpace::Core::SceneNodeGraph::CameraEvent, DrawSpace::Core::BaseSceneNode*>         CameraEvtCb;
    typedef DrawSpace::Core::CallBack2<DrawSpace::Planetoid::Body, void, DrawSpace::Core::SceneNodeGraph::NodesEvent, DrawSpace::Core::BaseSceneNode*>          NodesEventCb;
    typedef DrawSpace::Core::CallBack2<DrawSpace::Planetoid::Body, void, DrawSpace::Core::SceneNodeGraph::ScenegraphEvent, DrawSpace::Core::SceneNodeGraph*>    ScenegraphEventCb;
    
    typedef DrawSpace::Core::CallBack<DrawSpace::Planetoid::Body, void, DrawSpace::Core::PropertyPool*>                                                         RunnerMsgCb;
    typedef DrawSpace::Core::CallBack<DrawSpace::Planetoid::Body, void, DrawSpace::Core::Runner::State>                                                         RunnerEvtCb;
    
    typedef DrawSpace::Core::CallBack2<DrawSpace::Planetoid::Body, void, const std::vector<DrawSpace::SphericalLOD::Patch*>&, int >                             PatchsDrawRequestCb;

protected:

    typedef struct
    {
        bool                                        attached;

        bool                                        relative_alt_valid;
        dsreal                                      relative_alt;

        DrawSpace::Dynamics::InertBody*             body;
        Fragment*                                   fragment;
        DrawSpace::IntermediatePass*                collidingheightmap_pass;
        DrawSpace::SphericalLOD::FaceDrawingNode*   collidingheightmap_node;
        void*                                       collidingheightmap_content;

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

        Fragment*                           fragment;


    } RegisteredCamera;

    typedef struct
    {
        DrawSpace::Core::Texture*                   texture;
        void*                                       texture_content;
    
    } ProceduralTexture;

    typedef struct
    {
        DrawSpace::SphericalLOD::FaceDrawingNode*   reneringpatches_node;
        bool                                        need_redraw;
        DrawSpace::IntermediatePass*                pass;

    } SubPass;

    dsreal                                                                  m_ray;

    DrawSpace::Dynamics::World                                              m_world;
    dsstring                                                                m_scenename;
        
    DrawSpace::SphericalLOD::Drawing*                                       m_drawable;
    
    CameraEvtCb*                                                            m_camera_evt_cb;
    NodesEventCb*                                                           m_nodes_evt_cb;
    ScenegraphEventCb*                                                      m_scenegraph_evt_cb;
    PatchsDrawRequestCb*                                                    m_patchsdraw_request_cb;
      
    std::map<DrawSpace::Dynamics::InertBody*, RegisteredBody>               m_registered_bodies;
    std::map<dsstring, RegisteredCamera>                                    m_registered_camerapoints;
    dsstring                                                                m_current_camerapoint;

    std::vector<Fragment*>                                                  m_planetfragments_list;

    DrawSpace::Utils::Fractal*                                              m_fractal;

    std::map<DrawSpace::Pass*, std::vector<ProceduralTexture>>              m_procedural_global_textures;

    DrawSpace::Core::Runner*                                                m_proceduraltexture_runners[6];

    RunnerMsgCb*                                                            m_proceduraltexture_runnercb[6];
    RunnerEvtCb*                                                            m_proceduraltexture_runnerevt[6];


    DrawSpace::Pass*                                                        m_procedural_texture_currentpass;

    DrawSpace::Utils::TimeManager*                                          m_timemanager;

    // list of some passes to render for internal stuff
    // colliding heightmap, global textures ?
    std::vector<SubPass>                                                    m_subpasses;
    
    DrawSpace::IntermediatePass* create_colliding_heightmap_pass( const dsstring& p_inertbody_scenename );    

    void* create_colliding_heightmap( const dsstring& p_inertbody_scenename, DrawSpace::IntermediatePass** p_pass, DrawSpace::SphericalLOD::FaceDrawingNode** p_renderingnode );

    void attach_body( DrawSpace::Dynamics::InertBody* p_body );
    void detach_body( DrawSpace::Dynamics::InertBody* p_body );
    void body_find_attached_camera( DrawSpace::Dynamics::InertBody* p_body, std::vector<dsstring>& p_name );

    void on_camera_event( DrawSpace::Core::SceneNodeGraph::CameraEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );
    void on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );
    void on_scenegraph_event( DrawSpace::Core::SceneNodeGraph::ScenegraphEvent p_event, DrawSpace::Core::SceneNodeGraph* p_scenegraph );

    void create_camera_collisions( const dsstring& p_cameraname, DrawSpace::Dynamics::CameraPoint* p_camera, RegisteredCamera& p_cameradescr, bool p_hotstate );

    void apply_gravity( void );
    void manage_bodies( void );
    void manage_camerapoints( void );
    void update_fragments( void );

    void fill_procedural_texture( int p_direction, const ProceduralTexture& p_procedural_texture, DrawSpace::Utils::Fractal* p_fractal );

    void on_patchsdraw_request( const std::vector<DrawSpace::SphericalLOD::Patch*>& p_displaylist, int p_subpassindex );

    void on_proceduraltexture_request( DrawSpace::Core::PropertyPool* p_args );

    void on_front_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate );
    void on_rear_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate );
    void on_left_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate );
    void on_right_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate );
    void on_top_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate );
    void on_bottom_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate );
        

public:
    
    Body( const dsstring& p_scenename, dsreal p_ray, DrawSpace::Utils::TimeManager* p_time );
    virtual ~Body( void );

    void                                GetSceneName( dsstring& p_name );    
    void                                GetCameraHotpoint( const dsstring& p_name, DrawSpace::Utils::Matrix& p_outmat );

    void                                RegisterPlanetBodyPassSlot( Pass* p_pass );

    void                                RegisterSinglePassSlot( Pass* p_pass );

    DrawSpace::Core::RenderingNode*     GetPlanetBodyNodeFromPass( Pass* p_pass, int p_faceid );
    DrawSpace::Core::RenderingNode*     GetSingleNodeFromPass( Pass* p_pass );

    void                                BindPlanetBodyExternalGlobalTexture( DrawSpace::Core::Texture* p_texture, DrawSpace::Pass* p_pass, int p_faceid );
    void                                CreateProceduralGlobalTextures( DrawSpace::Pass* p_pass, int p_resol );
    void                                InitProceduralGlobalTextures( DrawSpace::Pass* p_pass, int p_r, int p_g, int p_b );
    
    Fragment*                           GetFragment( int p_index );

    //void                                AddPlanetBodyShader( DrawSpace::Pass* p_pass, int p_faceid, DrawSpace::Core::Shader* p_shader );

    DrawSpace::Core::Fx*                CreatePlanetBodyFx( DrawSpace::Pass* p_pass, int p_faceid );
    DrawSpace::Core::Fx*                CreateSingleNodeFx( DrawSpace::Pass* p_pass );

    void                                RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );

    void                                SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );
    void                                OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );
    void                                Update( DrawSpace::Utils::TimeManager& p_timemanager );
    void                                Update2( DrawSpace::Utils::TimeManager& p_timemanager );

    bool                                GetInertBodyRelativeAltitude( DrawSpace::Dynamics::InertBody* p_body, dsreal& p_rel_altitude );

    void                                InitNoisingTextures( void );

    void                                DrawSubPasses( void );
    
};



}
}


#endif
