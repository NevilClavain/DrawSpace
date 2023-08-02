/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

//fwd decl
struct FoliageConfig;

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
class Layer;


class CollisionMesheDrawingNode : public DrawSpace::Core::RenderingNode
{
public:
    CollisionMesheDrawingNode(DrawSpace::Interface::Renderer* p_renderer);
    ~CollisionMesheDrawingNode(void);

    void Draw(const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj);
    
private:
    DrawSpace::Interface::Renderer* m_renderer{ nullptr };
};

class FaceDrawingNode : public DrawSpace::Core::RenderingNode
{
public:

    enum class DrawPatchMode
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
    

    FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer, Config* p_config, int p_layer_index );
    virtual ~FaceDrawingNode( void );

    void Draw( dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Maths::Vector& p_invariant_view_pos,
                const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj, bool p_bind_ht_texture );

    void SetDisplayList( const std::vector<Patch*>& p_list );

    void SetCurrentBodyDescription( const dsstring& p_descr );
    
    void GetStats( Stats& p_stats ) const;

    void SetCurrentPatch( Patch* p_patch );

    void SetBinder( Binder* p_binder );

    Binder* GetBinder( void ) const;

    int GetLayerIndex( void ) const;

    void SetDrawPatchMode( DrawPatchMode p_mode, int maxlodlevel_to_draw = -1);

    void SetRenderStatePerPassTable(const std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>& p_table);

    void SetCurrentPass(const dsstring& p_pass);

    void UpdateRelativeHotPoint( const DrawSpace::Maths::Vector p_hotpoint );

private:

    DrawSpace::Interface::Renderer*                                                                             m_renderer{ nullptr };
    Config*                                                                                                     m_config;
    std::vector<Patch*>                                                                                         m_display_list;
    Binder*                                                                                                     m_binder{ nullptr };
    Stats                                                                                                       m_stats;
    Patch* m_current_patch{ nullptr };  // le connaitre pour eventuellement le dessiner d'une facon differente
    int                                                                                                         m_layer_index;
    DrawPatchMode                                                                                               m_drawpatch_mode{ DrawPatchMode::DRAW_ALL };
    DrawSpace::Maths::Vector                                                                                    m_relativehotpoint;
    dsstring                                                                                                    m_current_body_description; // for debug purpose only

    dsstring                                                                                                    m_current_pass;
    std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>      m_renderstate_per_passes;

    int                                                                                                         m_maxlodlevel_to_draw{ -1 }; // used in pair with DRAW_MAXLODLEVEL

    void                                draw_single_patch(Patch* p_patch, dsreal p_ray, dsreal p_rel_alt,
        const DrawSpace::Maths::Vector& p_invariant_view_pos,
        const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view,
        const DrawSpace::Maths::Matrix& p_proj);

    bool                                check_view_in_patch(dsreal p_ray, const DrawSpace::Maths::Vector& p_view, Patch* p_patch);
};


class FoliageDrawingNode : public DrawSpace::Core::RenderingNode
{
public:   

    struct CoordsGenerationParams
    {
        int                         nb_poles_min;
        int                         nb_poles_max;

        dsreal                      pole_ray_min;
        dsreal                      pole_ray_max;

        int                         nbpoints_per_pole_min;
        int                         nbpoints_per_pole_max;
    };

    FoliageDrawingNode(DrawSpace::Interface::Renderer* p_renderer, const FoliageConfig& p_config);

    void Draw(dsreal p_ray, LOD::Body* p_body, const DrawSpace::Maths::Vector& p_invariant_view_pos,
                        const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj);
    
    Binder* GetBinder(void) const;
    void SetBinder(Binder* p_binder);

   
    static const std::map<int, CoordsGenerationParams>& GetLocalSeeds(void);
    
private:

    DrawSpace::Interface::Renderer*                 m_renderer{ nullptr };
    Binder*                                         m_binder{ nullptr };

    bool                                            m_global_lit{ false };
    bool                                            m_detailed_lit{ false };

    // seed for this type of foliage
    int                                             m_local_seed{ 0 };

    dsreal                                          m_temperature_range_min;
    dsreal                                          m_temperature_range_max;

    dsreal                                          m_humidity_range_min;
    dsreal                                          m_humidity_range_max;

    dsreal                                          m_appearance;

    dsreal                                          m_altitud_max;

    // table of seeds/id foreach type of foliage
    //static std::set<int>                m_seeds;

    static std::map<int, CoordsGenerationParams>    m_seeds;


    void draw_foliages_batch_on_patch(Patch* p_patch, dsreal p_ray, 
                                        const DrawSpace::Maths::Vector& p_invariant_view_pos, const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj);

    void draw_foliage_on_patch(Patch* p_patch, dsreal p_ray, 
                                    const DrawSpace::Maths::Vector& p_invariant_view_pos, const DrawSpace::Maths::Matrix& p_world, const DrawSpace::Maths::Matrix& p_view, const DrawSpace::Maths::Matrix& p_proj, dsreal p_xpos, dsreal p_ypos, dsreal p_orientation);
};


class Drawing
{
private:

    using NodesSet = std::map<FaceDrawingNode*, int>;
    using RenderingNodeDrawCallback = DrawSpace::Core::CallBack<Drawing, void, DrawSpace::Core::RenderingNode*>;

public:

    using NewCollisionMesheCreationCb = DrawSpace::Core::CallBack<Drawing, void, const DrawSpace::Core::Meshe&>;

    Drawing( Config* p_config );
    virtual ~Drawing( void );

    void Startup( DrawSpace::Core::Entity* p_entity );
    void Shutdown( void );

    RenderingNodeDrawCallback* GetSingleNodeDrawHandler(void) const;

    void SetLayers(const std::vector<LOD::Layer*>& p_layers);

    void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );

    void SetLayerNodeDrawingState(int p_layer_index, bool p_drawing_state);


    void SetCurrentPass(const dsstring& p_pass);

    void AddInRendergraph(const dsstring& p_passname, DrawSpace::Core::RenderingQueue* p_passqueue);
    void RemoveFromRendergraph(const dsstring& p_passname, DrawSpace::Core::RenderingQueue* p_passqueue);

    void RegisterSinglePassSlot(const dsstring& p_pass, Binder* p_binder, int p_orientation, Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order, int maxlodlevel_to_draw = -1);
    void RegisterSinglePassSlotForCollisionDisplay(const dsstring& p_pass, DrawSpace::Core::Fx* p_fx, long p_rendering_order);


    void RegisterFoliageSinglePassSlot(const dsstring& p_pass, Binder* p_binder, int p_ro, int p_foliage_layer, const FoliageConfig& p_foliage_config);


    NewCollisionMesheCreationCb* GetNewCollisionMesheCreationCb(void) const;

    void ResetCollisionMesheValidity(void);

    void SetRenderStatePerPassTableForLayer(int p_layer_index, const std::map<dsstring, std::vector<std::pair<DrawSpace::Core::RenderState, DrawSpace::Core::RenderState>>>& p_table);


    std::vector<std::pair<dsstring, FaceDrawingNode*>> GetFaceDrawingNode(void) const;

private:

    std::vector<Body*>                                                          m_planetbodies;
    std::vector<LOD::Layer*>                                                    m_layers;

    std::vector<std::pair<dsstring, FaceDrawingNode*>>                          m_passesnodes;
    std::vector<std::pair<dsstring, FoliageDrawingNode*>>                       m_passesfoliagenodes;
    std::vector<FaceDrawingNode*>                                               m_facedrawingnodes;
    std::vector<FoliageDrawingNode*>                                            m_foliagedrawingnodes;

    std::vector<std::pair<dsstring, CollisionMesheDrawingNode*>>                m_passescollisionsdrawingnodes;
    std::vector<CollisionMesheDrawingNode*>                                     m_collisionmeshedrawingnodes;

    DrawSpace::Core::Meshe                                                      m_collisionmeshe;

    NodesSet                                                                    m_nodes;

    RenderingNodeDrawCallback* m_singlenode_draw_handler{ nullptr };

    std::vector<RenderingNodeDrawCallback*>                                     m_drawing_handlers;

    DrawSpace::Interface::Renderer* m_renderer{ nullptr };

    Config* m_config;

    DrawSpace::Core::Meshe* m_landplace_meshes[6];

    DrawSpace::Core::Entity* m_owner_entity{ nullptr };

    NewCollisionMesheCreationCb* m_newcollisionmeshecreation_cb{ nullptr };

    bool                                                                        m_collisionmeshe_valid{ false };

    std::map<FaceDrawingNode*, Patch*>                                          m_current_patchs; // current patch for each FaceDrawingNode

    void on_renderingnode_draw(DrawSpace::Core::RenderingNode* p_rendering_node);
    void on_rendering_singlenode_draw(DrawSpace::Core::RenderingNode* p_rendering_node);
    void on_collisionmeshe_draw(DrawSpace::Core::RenderingNode* p_rendering_node);

    void on_foliagerenderingnode_draw(DrawSpace::Core::RenderingNode* p_rendering_node);


    void create_landplace_meshe(long p_patch_resol, int p_orientation, DrawSpace::Core::Meshe* p_meshe_dest);

    void create_collision_meshe_from(const DrawSpace::Core::Meshe& p_src_meshe);   // for collision meshe display (debug feature)

    void create_all_landplace_meshes(void);
    void destroy_all_landplace_meshes(void);

    void on_new_collisionmeshe_creation(const DrawSpace::Core::Meshe& p_meshe);
};
}


