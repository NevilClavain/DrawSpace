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
public:

    CollisionMesheDrawingNode(DrawSpace::Interface::Renderer* p_renderer);

    ~CollisionMesheDrawingNode(void);

private:

    void draw(void);

};

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

    dsstring                            m_current_body_description; // for debug purpose only

    bool                                m_zbuffer_on{ false };

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

    void SetDrawPatchMode( DrawPatchMode p_mode );


    void UpdateRelativeHotPoint( const DrawSpace::Utils::Vector p_hotpoint );

    void EnableZBuffer(bool p_zbuffer);
    
};

class Drawing
{
protected:

    using NodesSet                  = std::map<FaceDrawingNode*, int>;
    using RenderingNodeDrawCallback = DrawSpace::Core::CallBack<Drawing, void, DrawSpace::Core::RenderingNode*>;

    std::vector<Body*>                                                          m_planetbodies;

    std::vector<std::pair<dsstring, FaceDrawingNode*>>                          m_passesnodes;

    std::vector<FaceDrawingNode*>                                               m_facedrawingnodes;

    NodesSet                                                                    m_nodes;

    RenderingNodeDrawCallback*                                                  m_singlenode_draw_handler;
    
    std::vector<RenderingNodeDrawCallback*>                                     m_drawing_handlers; 
    
    DrawSpace::Interface::Renderer*                                             m_renderer;
       
    Config*                                                                     m_config;

    DrawSpace::Core::Meshe*                                                     m_landplace_meshes[6];

    DrawSpace::Core::Entity*                                                    m_owner_entity;


    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );
    void on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );
    void on_collisionmeshe_draw(DrawSpace::Core::RenderingNode* p_rendering_node);

    void create_landplace_meshe( long p_patch_resol, int p_orientation, DrawSpace::Core::Meshe* p_meshe_dest );

    void create_all_landplace_meshes( void );
    void destroy_all_landplace_meshes( void );

public:

    Drawing( Config* p_config );
    virtual ~Drawing( void );

    void Startup( DrawSpace::Core::Entity* p_entity );
    void Shutdown( void );


    RenderingNodeDrawCallback* GetSingleNodeDrawHandler(void) const;

    void SetCurrentPlanetBodies( const std::vector<Body*>& p_planetbodies );
    void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );

    void SetLayerNodeDrawingState(int p_layer_index, bool p_drawing_state);
    void EnableZBufferForLayer(int p_layer_index, bool p_zbuffer);

    void AddInRendergraph(const dsstring& p_passname, DrawSpace::Core::RenderingQueue* p_passqueue);
    void RemoveFromRendergraph(const dsstring& p_passname, DrawSpace::Core::RenderingQueue* p_passqueue);

    void RegisterSinglePassSlot(const dsstring& p_pass, Binder* p_binder, int p_orientation, Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order );
    void RegisterSinglePassSlotForCollisionDisplay(const dsstring& p_pass);
   
};
}


