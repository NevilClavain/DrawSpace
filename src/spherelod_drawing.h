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

    DrawSpace::Interface::Renderer* m_renderer;
    //Face*                           m_face;

    std::vector<Patch*>             m_display_list;

    void*                           m_pnbufftexture_content;
    void*                           m_pnmaptexture_content;
    void*                           m_fbmexptexture_content;

    DrawSpace::Core::Texture*       m_perlinnoisebuffer_texture;
    DrawSpace::Core::Texture*       m_perlinnoisemap_texture;
    DrawSpace::Core::Texture*       m_fbmexp_texture;

    Stats                           m_stats;

    DrawSpace::Utils::Fractal*      m_fractal;

    void                            draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, 
                                                            const DrawSpace::Utils::Matrix& p_proj );
    
    

public:
    FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer );
    virtual ~FaceDrawingNode( void );

    void Draw( long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj );

    void SetDisplayList( const std::vector<Patch*>& p_list );
    
    void CreateNoisingTextures( void );
    void InitNoisingTextures( DrawSpace::Utils::Fractal* p_fractal );

    void GetStats( Stats& p_stats );

};


class Drawing
{
protected:

    typedef std::map<FaceDrawingNode*, int>                     NodesSet;

    typedef DrawSpace::Core::CallBack<Drawing, void, DrawSpace::Core::RenderingNode*> RenderingNodeDrawCallback;

    Body*                                                                       m_planetbody;

    std::map<Pass*, NodesSet>                                                   m_passesnodes;
    NodesSet                                                                    m_nodes;

    std::map<Pass*, SphericalLOD::FaceDrawingNode*>                             m_passes_singlenodes;

    std::vector<RenderingNodeDrawCallback*>                                     m_callbacks;
    DrawSpace::Interface::Renderer*                                             m_renderer;
    DrawSpace::Core::SceneNodeGraph*                                            m_scenenodegraph;
    
    Utils::Matrix                                                               m_globaltransformation;

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    void on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:

    Drawing( void );
    virtual ~Drawing( void );

    virtual void SetCurrentPlanetBody( Body* p_planetbody );

    virtual void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );

    virtual void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    virtual void RegisterPlanetBodyPassSlot( Pass* p_pass );
    virtual void RegisterSinglePassSlot( Pass* p_pass );

    DrawSpace::Core::RenderingNode* GetPlanetBodyNodeFromPass( Pass* p_pass, int p_faceid );
    DrawSpace::Core::RenderingNode* GetSingleNodeFromPass( Pass* p_pass );

    Body* GetBody( void );

    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );
    
    void InitNoisingTextures( DrawSpace::Utils::Fractal* p_fractal );

};
}
}

#endif
