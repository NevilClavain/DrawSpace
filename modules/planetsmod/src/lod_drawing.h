#pragma once

/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#include "renderer.h"

#include "lod_patch.h"
#include "lod_body.h"
#include "scenenodegraph.h"

namespace LOD
{
struct Config;
struct Binder;


class FaceDrawingNode : public DrawSpace::Core::RenderingNode
{
public:

    enum DrawPatchMode
    {
        DRAW_ALL,
        DRAW_LANDPLACEPATCH_ONLY,
        DRAW_ALL_BUTLANDPLACEPATCH
    
    };

    struct Stats
    {
        int                             nb_patchs;
    };

protected:

    DrawSpace::Interface::Renderer*     m_renderer;
    Config*                             m_config;

    std::vector<Patch*>                 m_display_list;

    Binder*                             m_binder;

    Stats                               m_stats;

    Patch*                              m_current_patch;  // le connaitre pour eventuellement le dessiner d'une facon differente

    int                                 m_layer_index;

    DrawPatchMode                       m_drawpatch_mode;

    DrawSpace::Utils::Vector            m_relativehotpoint;

    void                                draw_single_patch( Patch* p_patch, dsreal p_ray, dsreal p_rel_alt, 
                                                            const DrawSpace::Utils::Vector& p_invariant_view_pos,
                                                            const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, 
                                                            const DrawSpace::Utils::Matrix& p_proj );


    bool                                check_view_in_patch( dsreal p_ray, const DrawSpace::Utils::Vector& p_view, Patch* p_patch );
    
public:
    FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer, Config* p_config, int p_layer_index );
    virtual ~FaceDrawingNode( void );

    void Draw( dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Utils::Vector& p_invariant_view_pos, 
                const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj, bool p_bind_ht_texture );

    void SetDisplayList( const std::vector<Patch*>& p_list );
    
    void GetStats( Stats& p_stats );

    void SetCurrentPatch( Patch* p_patch );

    void SetBinder( Binder* p_binder );

    Binder* GetBinder( void );

    int GetLayerIndex( void );

    void SetDrawPatchMode( DrawPatchMode p_mode );


    void UpdateRelativeHotPoint( const DrawSpace::Utils::Vector p_hotpoint );
    
};

class Drawing
{
protected:

    using NodesSet                  = std::map<FaceDrawingNode*, int>;
    using RenderingNodeDrawCallback = DrawSpace::Core::CallBack<Drawing, void, DrawSpace::Core::RenderingNode*>;

    std::vector<Body*>                                                          m_planetbodies;

    std::vector<std::pair<DrawSpace::Pass*, FaceDrawingNode*> >                 m_passesnodes;

    std::vector<FaceDrawingNode*>                                               m_facedrawingnodes;

    NodesSet                                                                    m_nodes;

    RenderingNodeDrawCallback*                                                  m_singlenode_draw_handler;
    
    std::vector<RenderingNodeDrawCallback*>                                     m_drawing_handlers; 

    
    DrawSpace::Interface::Renderer*                                             m_renderer;
    DrawSpace::Core::SceneNodeGraph*                                            m_scenenodegraph;
    
    DrawSpace::Utils::Matrix                                                    m_globaltransformation;
    
    Config*                                                                     m_config;

    DrawSpace::Core::Meshe*                                                     m_landplace_meshes[6];

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    void on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    void create_landplace_meshe( long p_patch_resol, int p_orientation, DrawSpace::Core::Meshe* p_meshe_dest );

    void create_all_landplace_meshes( void );
    void destroy_all_landplace_meshes( void );

public:

    Drawing( Config* p_config );
    virtual ~Drawing( void );

    RenderingNodeDrawCallback* GetSingleNodeDrawHandler(void) const;

    void SetCurrentPlanetBodies( const std::vector<Body*>& p_planetbodies );
    void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );
    void SetFinalTransform(const DrawSpace::Utils::Matrix& p_mat);
    void SetLayerNodeDrawingState(int p_layer_index, bool p_drawing_state);


    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );
    void OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    void RegisterSinglePassSlot( DrawSpace::Pass* p_pass, Binder* p_binder, int p_orientation, Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order );
   
};
}


