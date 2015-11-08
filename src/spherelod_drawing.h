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
        int                             nb_hm_updates;

    } Stats;

protected:

    DrawSpace::Interface::Renderer* m_renderer;
    Face*                           m_face;

    DrawSpace::Core::Texture*       m_heighmap_texture;
    void*                           m_heighmaptexture_content;

    Stats                           m_stats;

    Patch*                          m_curr_hm;

    void                            draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, 
                                                            const DrawSpace::Utils::Matrix& p_proj );
    
    void                            update_heightmap( void );

    

public:
    FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer );
    virtual ~FaceDrawingNode( void );

    void Draw( long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj );
    void SetFace( Face* p_face );
    void CreateHeightMapTexture( void );
    void InitHeightMapTexture( void );
    void ClearHeightMapTexture( void );

    void GetStats( Stats& p_stats );

};


class Drawing
{
protected:

    typedef struct
    {
        FaceDrawingNode*        nodes[6];

    } NodesSet;

    typedef DrawSpace::Core::CallBack<Drawing, void, DrawSpace::Core::RenderingNode*> RenderingNodeDrawCallback;

    Body*                                                                       m_planetbody;


    std::map<Pass*, NodesSet>                                                   m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                                     m_callbacks;
    DrawSpace::Interface::Renderer*                                             m_renderer;
    DrawSpace::Core::SceneNodeGraph*                                            m_scenenodegraph;
    
    Utils::Matrix                                                               m_globaltransformation;

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:

    Drawing( void );
    virtual ~Drawing( void );

    virtual void SetCurrentPlanetBody( Body* p_planetbody );

    virtual void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );

    virtual void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );


    virtual void RegisterPassSlot( Pass* p_pass );

    DrawSpace::Core::RenderingNode* GetNodeFromPass( Pass* p_pass, int p_faceid );

    Body* GetBody( void );

    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );

    void InitHeightMapTextures( void );

};
}
}

#endif
