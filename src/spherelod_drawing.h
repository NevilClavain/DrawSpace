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

#ifndef _SPHERELOD_DRAWING_H_
#define _SPHERELOD_DRAWING_H_

#include "renderer.h"

#include "spherelod_body.h"
#include "scenenodegraph.h"


namespace DrawSpace
{
namespace SphericalLOD
{

class FaceDrawingNode : public DrawSpace::Core::RenderingNode
{
public:

    typedef struct
    {
        int                             nb_patchs;

    } Stats;

protected:

    DrawSpace::Interface::Renderer*     m_renderer;
    DrawSpace::SphericalLOD::Config*    m_config;

    std::vector<Patch*>                 m_display_list;

    DrawSpace::SphericalLOD::Binder*    m_binder;

    Stats                               m_stats;

    Patch*                              m_current_patch;  // le connaitre pour eventuellement le dessiner d'une facon differente

    int                                 m_layer_index;

    void                                draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, dsreal p_rel_alt, 
                                                            const DrawSpace::Utils::Vector& p_invariant_view_pos,
                                                            const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, 
                                                            const DrawSpace::Utils::Matrix& p_proj );
    
public:
    FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer, DrawSpace::SphericalLOD::Config* p_config, int p_layer_index );
    virtual ~FaceDrawingNode( void );

    void Draw( long p_nbv, long p_nbt, dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Utils::Vector& p_invariant_view_pos, 
                const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj, bool p_bind_ht_texture );

    void SetDisplayList( const std::vector<Patch*>& p_list );
    
    void GetStats( Stats& p_stats );

    void SetCurrentPatch( DrawSpace::SphericalLOD::Patch* p_patch );

    void SetBinder( DrawSpace::SphericalLOD::Binder* p_binder );

    DrawSpace::SphericalLOD::Binder* GetBinder( void );

    int GetLayerIndex( void );
    
};


class Drawing
{
protected:

    typedef std::map<FaceDrawingNode*, int>                                             NodesSet;
    typedef DrawSpace::Core::CallBack<Drawing, void, DrawSpace::Core::RenderingNode*>   RenderingNodeDrawCallback;

    std::vector<Body*>                                                          m_planetbodies;

    std::vector<std::pair<Pass*, FaceDrawingNode*> >                            m_passesnodes;

    std::vector<FaceDrawingNode*>                                               m_facedrawingnodes;

    NodesSet                                                                    m_nodes;

    RenderingNodeDrawCallback*                                                  m_singlenode_draw_handler;
    
    std::vector<RenderingNodeDrawCallback*>                                     m_drawing_handlers; 

    
    DrawSpace::Interface::Renderer*                                             m_renderer;
    DrawSpace::Core::SceneNodeGraph*                                            m_scenenodegraph;
    
    Utils::Matrix                                                               m_globaltransformation;
    
    DrawSpace::SphericalLOD::Config*                                            m_config;

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    void on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:

    Drawing( SphericalLOD::Config* p_config );
    virtual ~Drawing( void );

    virtual void SetCurrentPlanetBodies( const std::vector<Body*>& p_planetbodies );

    virtual void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );

    virtual void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );
    virtual void OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    virtual void RegisterSinglePassSlot( Pass* p_pass, SphericalLOD::Binder* p_binder, int p_orientation, DrawSpace::SphericalLOD::Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order );
   
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );

    RenderingNodeDrawCallback* GetSingleNodeDrawHandler( void );

    void SetLayerNodeDrawingState( int p_layer_index, bool p_drawing_state );
};
}
}

#endif
