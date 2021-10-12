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

#pragma once

#include "renderer.h"

#include "lod_patch.h"
#include "lod_body.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Core
{
class Entity;
}
};

namespace LOD
{
struct Config;
struct Binder;


class CollisionMesheDrawingNode : public DrawSpace::Core::RenderingNode
{
protected:

    DrawSpace::Interface::Renderer* m_renderer{ nullptr };


public:

    CollisionMesheDrawingNode(DrawSpace::Interface::Renderer* p_renderer);

    ~CollisionMesheDrawingNode(void);


    void Draw(const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj);

};

class FaceDrawingNode : public DrawSpace::Core::RenderingNode
{
public:

    enum DrawPatchMode
    {
        DRAW_ALL,
        DRAW_LANDPLACEPATCH_ONLY,
        DRAW_ALL_BUTLANDPLACEPATCH,
        DRAW_MAXLODLEVEL            // used in pair with m_maxlodlevel_to_draw
    };

    struct Stats
    {
        int                             nb_patchs;
    };

protected:

    DrawSpace::Interface::Renderer*                                                                             m_renderer;
    Config*                                                                                                     m_config;
    std::vector<Patch*>                                                                                         m_display_list;
    Binder*                                                                                                     m_binder;
    Stats                                                                                                       m_stats;
    Patch*                                                                                                      m_current_patch;  // le connaitre pour eventuellement le dessiner d'une facon differente
    int                                                                                                         m_layer_index;
    DrawPatchMode                                                                                               m_drawpatch_mode;
    DrawSpace::Utils::Vector                                                                                    m_relativehotpoint;
    dsstring                                                                                                    m_current_body_description; // for debug purpose only

    dsstring                                                                                                    m_current_pass;
    std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>      m_renderstate_per_passes;

    int                                                                                                         m_maxlodlevel_to_draw{ -1 }; // used in pair with DRAW_MAXLODLEVEL

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


    void SetCurrentBodyDescription( const dsstring& p_descr );
    
    void GetStats( Stats& p_stats );

    void SetCurrentPatch( Patch* p_patch );

    void SetBinder( Binder* p_binder );

    Binder* GetBinder( void );

    int GetLayerIndex( void );

    void SetDrawPatchMode( DrawPatchMode p_mode, int maxlodlevel_to_draw = -1);

    void SetRenderStatePerPassTable(const std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>& p_table);

    void SetCurrentPass(const dsstring& p_pass);

    void UpdateRelativeHotPoint( const DrawSpace::Utils::Vector p_hotpoint );

    /*
    void EnableZBuffer(bool p_zbuffer);
    void ForceCulling(const dsstring& p_culling);
    */
    
};

class Drawing
{
public:

    using NewCollisionMesheCreationCb = DrawSpace::Core::CallBack<Drawing, void, const DrawSpace::Core::Meshe&>;

protected:

    using NodesSet                  = std::map<FaceDrawingNode*, int>;
    using RenderingNodeDrawCallback = DrawSpace::Core::CallBack<Drawing, void, DrawSpace::Core::RenderingNode*>;

    std::vector<Body*>                                                          m_planetbodies;

    std::vector<std::pair<dsstring, FaceDrawingNode*>>                          m_passesnodes;
    std::vector<FaceDrawingNode*>                                               m_facedrawingnodes;

    std::vector<std::pair<dsstring, CollisionMesheDrawingNode*>>                m_passescollisionsdrawingnodes;
    std::vector<CollisionMesheDrawingNode*>                                     m_collisionmeshedrawingnodes;

    DrawSpace::Core::Meshe                                                      m_collisionmeshe;

    NodesSet                                                                    m_nodes;

    RenderingNodeDrawCallback*                                                  m_singlenode_draw_handler{ nullptr };
    
    std::vector<RenderingNodeDrawCallback*>                                     m_drawing_handlers; 
        
    DrawSpace::Interface::Renderer*                                             m_renderer;
       
    Config*                                                                     m_config;

    DrawSpace::Core::Meshe*                                                     m_landplace_meshes[6];

    DrawSpace::Core::Entity*                                                    m_owner_entity;

    NewCollisionMesheCreationCb*                                                m_newcollisionmeshecreation_cb{ nullptr };

    bool                                                                        m_collisionmeshe_valid{ false };

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );
    void on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );
    void on_collisionmeshe_draw(DrawSpace::Core::RenderingNode* p_rendering_node);

    void create_landplace_meshe( long p_patch_resol, int p_orientation, DrawSpace::Core::Meshe* p_meshe_dest );

    void create_collision_meshe_from(const DrawSpace::Core::Meshe& p_src_meshe);   // for collision meshe display (debug feature)

    void create_all_landplace_meshes( void );
    void destroy_all_landplace_meshes( void );

    void on_new_collisionmeshe_creation(const DrawSpace::Core::Meshe& p_meshe);

public:

    Drawing( Config* p_config );
    virtual ~Drawing( void );

    void Startup( DrawSpace::Core::Entity* p_entity );
    void Shutdown( void );

    RenderingNodeDrawCallback* GetSingleNodeDrawHandler(void) const;

    void SetCurrentPlanetBodies( const std::vector<Body*>& p_planetbodies );
    void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );

    void SetLayerNodeDrawingState(int p_layer_index, bool p_drawing_state);
    /*
    void EnableZBufferForLayer(int p_layer_index, bool p_zbuffer);
    void ForceCullingForLayer(int p_layer_index, const dsstring& p_culling);
    */

    void SetCurrentPass(const dsstring& p_pass);

    void AddInRendergraph(const dsstring& p_passname, DrawSpace::Core::RenderingQueue* p_passqueue);
    void RemoveFromRendergraph(const dsstring& p_passname, DrawSpace::Core::RenderingQueue* p_passqueue);

    void RegisterSinglePassSlot(const dsstring& p_pass, Binder* p_binder, int p_orientation, Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order, int maxlodlevel_to_draw = -1);
    void RegisterSinglePassSlotForCollisionDisplay(const dsstring& p_pass, DrawSpace::Core::Fx* p_fx, long p_rendering_order);

    NewCollisionMesheCreationCb* GetNewCollisionMesheCreationCb(void) const;

    void ResetCollisionMesheValidity(void);

    void SetRenderStatePerPassTableForLayer(int p_layer_index, const std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>& p_table);
};
}


