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

#include "transformnode.h"
#include "scenegraph.h"
#include "renderer.h"

#include "spherelod_body.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace SphericalLOD
{

class FaceDrawingNode : public DrawSpace::Core::RenderingNode
{    
protected:

    DrawSpace::Interface::Renderer* m_renderer;

    Face*                           m_face;

    void                            draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, 
                                                            const DrawSpace::Utils::Matrix& p_proj, DrawSpace::Utils::Vector& p_color );
    
public:
    FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer );
    virtual ~FaceDrawingNode( void );

    virtual void Draw( long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj );
    virtual void SetFace( Face* p_face );

};


class Drawing : public DrawSpace::Core::TransformNode
{
protected:

    typedef struct
    {
        FaceDrawingNode*        nodes[6];

    } NodesSet;

    typedef DrawSpace::Core::CallBack<Drawing, void, DrawSpace::Core::RenderingNode*> RenderingNodeDrawCallback;

    Body*                                                                       m_planetbody;


    std::map<dsstring, NodesSet>                                                m_passesnodes;
    std::vector<RenderingNodeDrawCallback*>                                     m_callbacks;
    DrawSpace::Interface::Renderer*                                             m_renderer;
    DrawSpace::Scenegraph*                                                      m_scenegraph;
    DrawSpace::Core::SceneNodeGraph*                                            m_scenenodegraph;
    DrawSpace::Core::Fx*                                                        m_fx;


    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

public:

    Drawing( void );
    virtual ~Drawing( void );

    virtual void SetCurrentPlanetBody( Body* p_planetbody );

    virtual void SetRenderer( DrawSpace::Interface::Renderer * p_renderer );
    virtual void OnRegister( DrawSpace::Scenegraph* p_scenegraph );
    virtual void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node );

    virtual void RegisterPassSlot( const dsstring& p_passname );
    DrawSpace::Core::RenderingNode* GetNodeFromPass( const dsstring& p_passname, int p_faceid );
    void SetNodeFromPassSpecificFx( const dsstring& p_passname, int p_faceid, const dsstring& p_fxname );

    Body* GetBody( void );

    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat );

};
}
}

#endif
